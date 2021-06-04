// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenExcelComponent.h"
#include <stdio.h>
#include <locale>
#include <codecvt>
#include "Misc/Paths.h"
#include <sstream>
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Misc/FileHelper.h"
#include <regex>
#include "MESProduct/Tool/MESPathUtility.h"
#include "MESProduct/Tool/MESFileUtility.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/FileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"


//#define INSERT_SELECT_BATCH_COUNT 450 // it will occur error over 500 ?

class chs_codecvt : public std::codecvt_byname<wchar_t, char, std::mbstate_t> {
public:
	chs_codecvt() : codecvt_byname("chs") { }
};

// Sets default values for this component's properties
UOpenExcelComponent::UOpenExcelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UOpenExcelComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UOpenExcelComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SqliteConnection.Close();
	Super::EndPlay(EndPlayReason);
}

void UOpenExcelComponent::ImportToDBFromExcel(FString ExcelFullPath, FString DBFullPath)
{
	FString DBFileExtension = FPaths::GetExtension(DBFullPath, true);
	if (!DBFileExtension.Equals(".db"))
	{
		UE_LOG(LogTemp, Warning, TEXT("ImportToDBFromExcel, unknown db extension, not support"));
		return;
	}

	FString ExcelFileExtension = FPaths::GetExtension(ExcelFullPath, true);
	if (ExcelFileExtension.Equals(".csv"))
	{
		ProcessCsv(ExcelFullPath, DBFullPath);
	}
	else if (ExcelFileExtension.Equals(".xlsx"))
	{
		ProcessExcel(ExcelFullPath, DBFullPath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ImportToDBFromExcel, unknown excel extension, not support"));
	}
}

void UOpenExcelComponent::ImportExcelToDB(FString ExcelFullPath, FString ProjectName)
{
	FString DBDirectory = UMESPathUtility::GetStandaloneProjectDataPath(ProjectName);
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectory(*DBDirectory);

	FString ExcelFileName = FPaths::GetBaseFilename(ExcelFullPath);
	if (ExcelFileName.IsEmpty())
	{
		return;
	}
	FString DBFullPath = DBDirectory + ExcelFileName + ".db";

	FString ConfirmMessage = TEXT("该Excel数据已存在，是否覆盖?");
	if (PlatformFile.FileExists(*DBFullPath))
	{
		if (!UMESFileUtility::OpenConfirmDialog(ConfirmMessage))
		{
			return;
		}
		else
		{
			PlatformFile.DeleteFile(*DBFullPath);
		}
	}
	UMESFileUtility::CreateFileWithFullPath(DBFullPath);

	FString ExcelFileExtension = FPaths::GetExtension(ExcelFullPath, true);
	if (ExcelFileExtension.Equals(".csv"))
	{
		ProcessCsv(ExcelFullPath, DBFullPath);
	}
	else if (ExcelFileExtension.Equals(".xlsx"))
	{
		ProcessExcel(ExcelFullPath, DBFullPath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ImportToDBFromExcel, unknown excel extension, not support"));
	}
}

void UOpenExcelComponent::ProcessExcel(const FString ExcelFullPath, FString DBFullPath)
{
	FDateTime OpenExcelStart = FDateTime::Now();

	OpenXLSX::XLDocument XDoc;
	std::string convStr = ConvertToString(ExcelFullPath);
	XDoc.OpenDocument(convStr);

	FDateTime OpenExcelEnd = FDateTime::Now();
	FTimespan OpenExcelTime = OpenExcelEnd - OpenExcelStart;

	FString ExcelFileName = FPaths::GetBaseFilename(ExcelFullPath);
	std::string FileName = TCHAR_TO_UTF8(*ExcelFileName);

	TArray<FString> InsertSqls;
	TArray<FString> CreateTableSqls;

	for (std::string SheetName : XDoc.Workbook().WorksheetNames())
	{
		FDateTime ReadExcelStart = FDateTime::Now();

		bool HasCorrectDataTypeRow = false;

		OpenXLSX::XLWorksheet XWorkSheet = XDoc.Workbook().Worksheet(SheetName);
		unsigned long XRowCount = XWorkSheet.RowCount();
		unsigned int XColumnCount = XWorkSheet.ColumnCount();

		// deal with title/header and type.
		// row 1 is title/header which represent column of data table in db.
		// row 2 is used to specify the data type of current column. if not, then all the data type will be treat as string and store into sqlite db.
		std::string DataTableName = FileName + "_" + SheetName;

		std::stringstream CreateTableSql;
		CreateTableSql << "create table ";
		CreateTableSql << DataTableName;
		CreateTableSql << " (";

		std::stringstream InsertSql;
		InsertSql << "insert into ";
		InsertSql << DataTableName;
		InsertSql << " (";

		// batch insert str : insert into ... select ... union all select ...
		// create table(fieldname fieldtype, ...)
		// combine sql of create table and insert into
		for (unsigned int ColumnIndex = 1; ColumnIndex <= XColumnCount; ++ColumnIndex)
		{
			std::string TableHeader = XWorkSheet.Cell(1, ColumnIndex).Value().AsString();

			CreateTableSql << TableHeader;
			CreateTableSql << " ";

			// the second row was assumed to represent data type of its column.
			// we now support int, string, double
			// Otherwise, the data type will be as string.
			std::string WannaDataType = XWorkSheet.Cell(2, ColumnIndex).Value().AsString();
			FString TypeToCompare(WannaDataType.c_str());
			if (CheckDataType(TypeToCompare))
			{
				CreateTableSql << WannaDataType;
				HasCorrectDataTypeRow = true;
			}
			else
			{
				CreateTableSql << "string";
			}

			InsertSql << TableHeader;
			if (ColumnIndex < XColumnCount)
			{
				InsertSql << ",";
				CreateTableSql << ",";
			}
			else
			{
				InsertSql << ")";
				CreateTableSql << ")";
			}
		}

		// combine the first select sql of insert into
		unsigned long ContentStartRow = HasCorrectDataTypeRow ? 3 : 2;
		InsertSql << " values (";
		for (unsigned int ColumnIndex = 1; ColumnIndex <= XColumnCount; ++ColumnIndex)
		{
			InsertSql << "'";
			InsertSql << XWorkSheet.Cell(ContentStartRow, ColumnIndex).Value().AsString();
			InsertSql << "'";

			if (ColumnIndex < XColumnCount)
			{
				InsertSql << ",";
			}
			else
			{
				if (ContentStartRow == XRowCount)
				{
					InsertSql << ")";
				}
				else
				{
					InsertSql << "),";
				}
			}
		}

		++ContentStartRow;
		for (unsigned long RowIndex = ContentStartRow; RowIndex <= XRowCount; ++RowIndex)
		{
			InsertSql << "(";
			for (unsigned int ColumnIndex = 1; ColumnIndex <= XColumnCount; ++ColumnIndex)
			{
				InsertSql << "'";
				InsertSql << XWorkSheet.Cell(RowIndex, ColumnIndex).Value().AsString();
				InsertSql << "'";

				if (ColumnIndex < XColumnCount)
				{
					InsertSql << ",";
				}
			}
			if (RowIndex < XRowCount)
			{
				InsertSql << "),";
			}
			else
			{
				InsertSql << ")";
			}
		}

		FDateTime ReadExcelEnd = FDateTime::Now();
		FTimespan ReadExcelTime = ReadExcelEnd - ReadExcelStart;

		InsertSqls.Add(UTF8_TO_TCHAR(InsertSql.str().c_str()));
		CreateTableSqls.Add(UTF8_TO_TCHAR(CreateTableSql.str().c_str()));

		UE_LOG(LogTemp, Log, TEXT("open excel time (milliseconds): %f"), OpenExcelTime.GetTotalMilliseconds());
		UE_LOG(LogTemp, Log, TEXT("read excel time (milliseconds): %f"), ReadExcelTime.GetTotalMilliseconds());
	}
	ImportToDB(CreateTableSqls, InsertSqls, DBFullPath);
}

void UOpenExcelComponent::ProcessCsv(const FString CsvFullPath, FString DBFullPath)
{
	FDateTime OpenCsvStart = FDateTime::Now();
	TArray<FString> FileContents;
	FFileHelper::LoadFileToStringArray(FileContents, *CsvFullPath);

	/*TArray<uint8> FileBuffer;
	FFileHelper::LoadFileToArray(FileBuffer, *CsvFullPath);
	char* CsvChar = reinterpret_cast<char*>(FileBuffer.GetData());
	std::string CsvStr(CsvChar);

	std::string Delimiter = "\r\n";
	std::vector<std::string> FileContents = StringSplit(CsvStr, Delimiter);*/


	/*TArray<uint8> testArray;
	FFileHelper::LoadFileToArray(testArray, *CsvFullPath);
	uint8* testArrayData = testArray.GetData();
	char* testChardata = reinterpret_cast<char*>(testArrayData);
	std::string testCharStr(testChardata);
	FString ddFStr(ANSI_TO_TCHAR(testChardata));
	FString testCharFStr(UTF8_TO_TCHAR(testCharStr.c_str()));
	FString testttt(ANSI_TO_TCHAR(testChardata));
	FString sss(testChardata);
	FString ttt = FString::Printf(TEXT("%s"), testChardata);
	wchar_t* testWChardata = reinterpret_cast<wchar_t*>(testArrayData);
	std::wstring testWdata = testWChardata;
	FString ff(WCHAR_TO_TCHAR(testWChardata));

	FString testContent;
	FFileHelper::LoadFileToString(testContent, *CsvFullPath);
	std::string testStr = TCHAR_TO_UTF8(*testContent);
	FString dd(UTF8_TO_TCHAR(testStr.c_str()));
	std::wstring_convert<chs_codecvt> converter;
	std::wstring testWChar = converter.from_bytes(testStr);

	const wchar_t* WideChar = TCHAR_TO_WCHAR(*testContent);
	std::wstring WideStr = WideChar;

	std::string ee((const ANSICHAR*)testArrayData);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	std::wstring eef = conv.from_bytes(ee);
	FString eff(eef.c_str());*/

	FDateTime OpenCsvEnd = FDateTime::Now();
	FTimespan OpenCsvTime = OpenCsvEnd - OpenCsvStart;

	FDateTime ReadCsvStart = FDateTime::Now();

	FString CsvFileName = FPaths::GetBaseFilename(CsvFullPath);
	FString DataTableName = CsvFileName + "_Csv";
	//std::string DataTableName = ConvertToString(CsvFileName);

	std::stringstream CreateTableSql;
	CreateTableSql << "create table ";
	CreateTableSql << TCHAR_TO_UTF8(*DataTableName);
	CreateTableSql << " (";

	std::stringstream InsertSql;
	InsertSql << "insert into ";
	InsertSql << TCHAR_TO_UTF8(*DataTableName);
	InsertSql << " (";

	// batch insert str : insert into ... select ... union all select ...
	// create table(fieldname fieldtype, ...)
	// combine sql of create table and insert into


	// deal with first row, the table header
	if (FileContents.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("csv file has no correct header and type"));
		return;
	}

	TCHAR* CsvDelimite = TEXT(",");
	FString FirstRow = FileContents[0];
	FString SecondRow = FileContents[1];
	TArray<FString> FirstRowContents;
	TArray<FString> SecondRowContents;
	FirstRow.ParseIntoArray(FirstRowContents, CsvDelimite);
	SecondRow.ParseIntoArray(SecondRowContents, CsvDelimite);

	/*std::string CsvDelimiter = ",";
	std::string FirstRow = FileContents[0];
	std::string SecondRow = FileContents[1];
	std::vector<std::string> FirstRowContents = StringSplit(FirstRow, CsvDelimiter);
	std::vector<std::string> SecondRowContents = StringSplit(SecondRow, CsvDelimiter);*/

	bool HasCorrectDataTypeRow = false;

	for (int i = 0; i < FirstRowContents.Num() && i < SecondRowContents.Num(); ++i)
	{
		FString TableHeader = FirstRowContents[i];

		CreateTableSql << TCHAR_TO_UTF8(*TableHeader);
		CreateTableSql << " ";

		// the second row was assumed to represent data type of its column.
		// we now support int, string, double
		// Otherwise, the data type will be as string.

		if (CheckDataType(SecondRowContents[i]))
		{
			CreateTableSql << TCHAR_TO_UTF8(*SecondRowContents[i]);
			HasCorrectDataTypeRow = true;
		}
		else
		{
			CreateTableSql << "string";
		}

		InsertSql << TCHAR_TO_UTF8(*TableHeader);
		if (i < FirstRowContents.Num() - 1 && i < SecondRowContents.Num() - 1)
		{
			InsertSql << ",";
			CreateTableSql << ",";
		}
		else
		{
			InsertSql << ")";
			CreateTableSql << ")";
		}
	}

	// combine the first select sql of insert into

	int ContentStartRowIndex = HasCorrectDataTypeRow ? 2 : 1;
	if (FileContents.Num() < ContentStartRowIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("csv file has no correct data"));
		return;
	}
	InsertSql << " values (";

	//std::vector<std::string> RowContents = StringSplit(FileContents[ContentStartRowIndex], CsvDelimiter);
	TArray<FString> RowContents;
	FileContents[ContentStartRowIndex].ParseIntoArray(RowContents, CsvDelimite);
	for (int ColumnIndex = 0; ColumnIndex < RowContents.Num(); ++ColumnIndex)
	{
		InsertSql << "'";
		InsertSql << TCHAR_TO_UTF8(*RowContents[ColumnIndex]);
		InsertSql << "'";

		if (ColumnIndex < RowContents.Num() - 1)
		{
			InsertSql << ",";
		}
		else
		{
			if (ContentStartRowIndex == FileContents.Num())
			{
				InsertSql << ")";
			}
			else
			{
				InsertSql << "),";
			}
		}
	}

	++ContentStartRowIndex;
	for (int RowIndex = ContentStartRowIndex; RowIndex < FileContents.Num(); ++RowIndex)
	{
		InsertSql << "(";

		/*RowContents.clear();
		RowContents = StringSplit(FileContents[RowIndex], CsvDelimiter);*/
		RowContents.Empty();
		FileContents[RowIndex].ParseIntoArray(RowContents, CsvDelimite);
		for (int ColumnIndex = 0; ColumnIndex < RowContents.Num(); ++ColumnIndex)
		{
			InsertSql << "'";
			InsertSql << TCHAR_TO_UTF8(*RowContents[ColumnIndex]);
			InsertSql << "'";

			if (ColumnIndex < RowContents.Num() - 1)
			{
				InsertSql << ",";
			}
		}
		if (RowIndex < FileContents.Num() - 1)
		{
			InsertSql << "),";
		}
		else
		{
			InsertSql << ")";
		}
	}

	FDateTime ReadCsvEnd = FDateTime::Now();
	FTimespan ReadCsvTime = ReadCsvEnd - ReadCsvStart;

	TArray<FString> InsertSqls;
	TArray<FString> CreateTableSqls;
	InsertSqls.Add(UTF8_TO_TCHAR(InsertSql.str().c_str()));
	CreateTableSqls.Add(UTF8_TO_TCHAR(CreateTableSql.str().c_str()));

	/*std::vector<std::string> InsertSqls;
	std::vector<std::string> CreateTableSqls;
	InsertSqls.push_back(InsertSql.str().c_str());
	CreateTableSqls.push_back(CreateTableSql.str().c_str());*/

	UE_LOG(LogTemp, Log, TEXT("open csv time (milliseconds): %f"), OpenCsvTime.GetTotalMilliseconds());
	UE_LOG(LogTemp, Log, TEXT("read csv time (milliseconds): %f"), ReadCsvTime.GetTotalMilliseconds());
	
	ImportToDB(CreateTableSqls, InsertSqls, DBFullPath);
}

void UOpenExcelComponent::ImportToDB(TArray<FString> CreateTableSqls, TArray<FString> InsertSqls, FString DBFullPath)
{
	// open database and execute sql, create table and insert data into table.
	if (!SqliteConnection.Open(*DBFullPath, nullptr, nullptr))
	{
		UE_LOG(LogTemp, Log, TEXT("Can not open sqlite with path: %s"), *DBFullPath);
		return;
	}

	FDateTime CreateTableStart = FDateTime::Now();
	for (FString CreateTableSql : CreateTableSqls)
	{
		if (!SqliteConnection.Execute(*CreateTableSql))
		{
			UE_LOG(LogTemp, Log, TEXT("create table failed : %s"), *CreateTableSql);
			SqliteConnection.Close();
			break;
		}
	}
	
	FDateTime CreateTableEnd = FDateTime::Now();
	FTimespan CreateTableTime = CreateTableEnd - CreateTableStart;

	FDateTime InsertStart = FDateTime::Now();
	for (FString InsertStr : InsertSqls)
	{
		if (!SqliteConnection.Execute(*InsertStr))
		{
			UE_LOG(LogTemp, Log, TEXT("insert data failed: %s"), *InsertStr);
			SqliteConnection.Close();
			break;
		}
	}

	FDateTime InsertEnd = FDateTime::Now();
	FTimespan InsertTime = InsertEnd - InsertStart;

	UE_LOG(LogTemp, Log, TEXT("create table time (milliseconds): %f"), CreateTableTime.GetTotalMilliseconds());
	UE_LOG(LogTemp, Log, TEXT("insert total time (milliseconds): %f"), InsertTime.GetTotalMilliseconds());

	SqliteConnection.Close();
}

void UOpenExcelComponent::ImportToDB(std::vector<std::string> CreateTableSqls, std::vector<std::string> InsertSqls, FString DBFullPath)
{
	// open database and execute sql, create table and insert data into table.
	if (!SqliteConnection.Open(*DBFullPath, nullptr, nullptr))
	{
		UE_LOG(LogTemp, Log, TEXT("Can not open sqlite with path: %s"), *DBFullPath);
		return;
	}

	FDateTime CreateTableStart = FDateTime::Now();
	for (std::string CreateTableSql : CreateTableSqls)
	{
		const TCHAR *  c = UTF8_TO_TCHAR(CreateTableSql.c_str());
		if (!SqliteConnection.Execute(c))
		{
			UE_LOG(LogTemp, Log, TEXT("create table failed : %s"), CreateTableSql.c_str());
			SqliteConnection.Close();
			break;
		}
	}

	FDateTime CreateTableEnd = FDateTime::Now();
	FTimespan CreateTableTime = CreateTableEnd - CreateTableStart;

	FDateTime InsertStart = FDateTime::Now();
	for (std::string InsertStr : InsertSqls)
	{
		const TCHAR *  c = UTF8_TO_TCHAR(InsertStr.c_str());
		if (!SqliteConnection.Execute(c))
		{
			UE_LOG(LogTemp, Log, TEXT("insert data failed: %s"), InsertStr.c_str());
			SqliteConnection.Close();
			break;
		}
	}

	FDateTime InsertEnd = FDateTime::Now();
	FTimespan InsertTime = InsertEnd - InsertStart;

	UE_LOG(LogTemp, Log, TEXT("create table time (milliseconds): %f"), CreateTableTime.GetTotalMilliseconds());
	UE_LOG(LogTemp, Log, TEXT("insert total time (milliseconds): %f"), InsertTime.GetTotalMilliseconds());

	SqliteConnection.Close();
}

bool UOpenExcelComponent::CheckDataType(FString CellContent)
{
	if (CellContent.Equals("int") || CellContent.Equals("string") || CellContent.Equals("double"))
	{
		return true;
	}
	return false;
}

bool UOpenExcelComponent::CheckDataType(std::string CellContent)
{
	if (CellContent.compare("int") || CellContent.compare("string") || CellContent.compare("double"))
	{
		return true;
	}
	return false;
}

std::string UOpenExcelComponent::ConvertToString(FString Str)
{
	const wchar_t* WideChar = TCHAR_TO_WCHAR(*Str);
	std::wstring WideStr = WideChar;
	std::wstring_convert<chs_codecvt> converter;
	std::string Result = converter.to_bytes(WideStr);
	return Result;
}

std::vector<std::string> UOpenExcelComponent::StringSplit(const std::string& input, const std::string& regex) {
	//std::stringstream ss(Source);
	//std::string item;
	//std::vector<std::string> elems;
	//while (std::getline(ss, item, Delimiter)) {
	//	//elems.push_back(item);
	//	elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
	//}
	//return elems;

	// passing -1 as the submatch index parameter performs splitting
	std::regex re(regex);
	std::sregex_token_iterator
		first{ input.begin(), input.end(), re, -1 },
		last;
	return { first, last };
}