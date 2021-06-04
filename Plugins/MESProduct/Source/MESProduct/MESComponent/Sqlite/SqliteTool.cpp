// Fill out your copyright notice in the Description page of Project Settings.


#include "SqliteTool.h"
#include "Misc/Paths.h"


// Sets default values for this component's properties
USqliteTool::USqliteTool()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USqliteTool::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
FSQLiteDatabaseConnection USqliteTool::SqliteConnect = FSQLiteDatabaseConnection();
TMap<FString, TArray<USqliteTool::FChangeDataInfo>> USqliteTool::ChangedDatas = TMap<FString, TArray<USqliteTool::FChangeDataInfo>>();

void USqliteTool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SqliteConnect.Close();
	Super::EndPlay(EndPlayReason);
}

void USqliteTool::LoadDataForCpp(FString DBPath, FString SelectSqlString, FString DataTableName, FSqliteData& Result)
{
	Result.Headers = GetDataTableHeaderForCpp(DBPath, DataTableName);

	TArray<FSqliteRowData> RowDatas;
	if (SqliteConnect.Open(*DBPath, nullptr, nullptr))
	{
		FSQLiteResultSet* SqlResults = NULL;
		if (SqliteConnect.Execute(*SelectSqlString, SqlResults))
		{
			TArray<FDatabaseColumnInfo> Columns = SqlResults->GetColumnNames();
			for (FSQLiteResultSet::TIterator SqlResult(SqlResults); SqlResult; ++SqlResult)
			{
				TArray<FString> ColumnDatas;
				for (FDatabaseColumnInfo Column : Columns)
				{
					ColumnDatas.Add(SqlResult->GetString(*Column.ColumnName));
				}
				FSqliteRowData RowData;
				RowData.ColumnDatas = ColumnDatas;
				RowDatas.Add(RowData);
			}
			delete SqlResults;
			SqlResults = nullptr;
		}
	}
	Result.RowDatas = RowDatas;
	LogSqliteError(SelectSqlString);
	SqliteConnect.Close();
}


TArray<FString> USqliteTool::GetDataTableHeaderForCpp(FString DBPath, FString DataTableName)
{
	TArray<FString> Headers;
	if (SqliteConnect.Open(*DBPath, nullptr, nullptr))
	{
		FSQLiteResultSet* SqlResults = nullptr;
		FString SqlStr = "pragma table_info([" + DataTableName + "])";
		if (SqliteConnect.Execute(*SqlStr, SqlResults))
		{
			TArray<FDatabaseColumnInfo> Columns = SqlResults->GetColumnNames();
			for (FSQLiteResultSet::TIterator SqlResult(SqlResults); SqlResult; ++SqlResult)
			{
				for (FDatabaseColumnInfo Column : Columns)
				{
					if (Column.ColumnName.Equals("name"))
					{
						Headers.Add(SqlResult->GetString(*Column.ColumnName));
					}
				}
			}
			delete SqlResults;
			SqlResults = nullptr;
		}
	}
	LogSqliteError(TEXT("GetDataTableHeader"));
	SqliteConnect.Close();
	return Headers;
}

void USqliteTool::LoadTableData(FString DBPath, FString SelectSqlString, FString DataTableName, FTableData& Result)
{
	FString DBName = FPaths::GetBaseFilename(DBPath);
	DBName.TrimStartAndEndInline();
	if (DBName.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("invalid db Path, has no db name"));
		return;
	}

	Result.Headers.Empty();
	Result.RowDatas.Empty();
	TArray<FDatabaseColumnInfo> Columns;
	GetColumnName(DBPath, SelectSqlString,Columns);
	if (Columns.Num() == 0)
	{
		return;
	}
	for (int i = 0; i < Columns.Num(); i++)
	{
		FTableFiled header;
		header.FieldName = Columns[i].ColumnName;
		header.FieldIndex = i;//TODO 后面这个Index 要从 整表里面去取
		header.HeaderType = "String";
		Result.Headers.Add(header);
	}

	TArray<FTableRowData> RowDatas;
	if (SqliteConnect.Open(*DBPath, nullptr, nullptr))
	{
		FSQLiteResultSet* SqlResults = NULL;
		if (SqliteConnect.Execute(*SelectSqlString, SqlResults))
		{
			for (FSQLiteResultSet::TIterator SqlResult(SqlResults); SqlResult; ++SqlResult)
			{
				TArray<FString> ColumnDatas;
				for (FDatabaseColumnInfo Column : Columns)
				{
					ColumnDatas.Add(SqlResult->GetString(*Column.ColumnName));
				}
				FTableRowData RowData;
				RowData.ColumnDatas = ColumnDatas;
				RowDatas.Add(RowData);
			}
			delete SqlResults;
			SqlResults = nullptr;
		}
	}
	Result.RowDatas = RowDatas;
	LogSqliteError(SelectSqlString);
	SqliteConnect.Close();
}

void USqliteTool::GetColumnName(FString DBPath,FString Sql, TArray<FDatabaseColumnInfo>& ColumnInfos)
{
	FSQLiteDatabase Database;
	if (!Database.Open(*DBPath))
	{
		Database.Close();
		return;
	}
	if (Database.IsValid())
	{
		// Compile the statement/query
		FSQLitePreparedStatement PreparedStatement = Database.PrepareStatement(*Sql);
		if (PreparedStatement.IsValid())
		{
			const TArray<FString>& ColumnNames = PreparedStatement.GetColumnNames();
			for (int32 ColumnIndex = 0; ColumnIndex < ColumnNames.Num(); ++ColumnIndex)
			{
				FDatabaseColumnInfo& ColumnInfo = ColumnInfos.AddDefaulted_GetRef();
				ColumnInfo.ColumnName = ColumnNames[ColumnIndex];

				ESQLiteColumnType ColumnType = ESQLiteColumnType::Null;
				PreparedStatement.GetColumnTypeByIndex(ColumnIndex, ColumnType);
				switch (ColumnType)
				{
				case ESQLiteColumnType::Integer:
					ColumnInfo.DataType = DBT_INT;
					break;

				case ESQLiteColumnType::Float:
					ColumnInfo.DataType = DBT_FLOAT;
					break;

				case ESQLiteColumnType::String:
					ColumnInfo.DataType = DBT_STRING;
					break;

				case ESQLiteColumnType::Blob:
					ColumnInfo.DataType = DBT_UNKOWN;
					break;

				case ESQLiteColumnType::Null:
				default:
					ColumnInfo.DataType = DBT_UNKOWN;
					break;
				}
			}
		}
	}
	Database.Close();
}

FSqliteData USqliteTool::LoadLocalData(FString DBPath, FString DataTableName)
{
	FSqliteData Result;
	Result.Headers = GetDataTableHeader(DBPath, DataTableName);

	TArray<FSqliteRowData> RowDatas;
	if (SqliteConnect.Open(*DBPath, nullptr, nullptr))
	{
		FSQLiteResultSet* SqlResults = NULL;
		FString SelectSql = "select * from " + DataTableName;
		if (SqliteConnect.Execute(*SelectSql, SqlResults))
		{
			TArray<FDatabaseColumnInfo> Columns = SqlResults->GetColumnNames();
			for (FSQLiteResultSet::TIterator SqlResult(SqlResults); SqlResult; ++SqlResult)
			{
				TArray<FString> ColumnDatas;
				for (FDatabaseColumnInfo Column : Columns)
				{
					ColumnDatas.Add(SqlResult->GetString(*Column.ColumnName));
				}
				FSqliteRowData RowData;
				RowData.ColumnDatas = ColumnDatas;
				RowDatas.Add(RowData);
			}
			delete SqlResults;
			SqlResults = nullptr;
		}
	}
	Result.RowDatas = RowDatas;
	LogSqliteError("LoadLocalData");
	SqliteConnect.Close();
	
	return Result;
}

FSqliteData USqliteTool::LoadDataWithoutHeader(FString DBPath, FString SelectSqlString)
{
	return LoadDataIncludeHeader(DBPath, SelectSqlString, "");
}

FSqliteData USqliteTool::LoadDataIncludeHeader(FString DBPath, FString SelectSqlString, FString DataTableName)
{
	FSqliteData result;
	USqliteTool::LoadDataForCpp(DBPath, SelectSqlString, DataTableName,result);
	return result;
}

bool USqliteTool::ExecuteSqlString(FString DBPath, TArray<FString> SqlStrings)
{
	if (SqlStrings.Num() <= 0)
	{
		return false;
	}

	bool HasAnyFail = false;
	if (SqliteConnect.Open(*DBPath, nullptr, nullptr))
	{
		for (FString SqlString : SqlStrings)
		{
			if (!SqliteConnect.Execute(*SqlString))
			{
				HasAnyFail = true;
				LogSqliteError(SqlString);
			}
		}
	}
	else
	{
		HasAnyFail = true;
	}
	SqliteConnect.Close();
	return !HasAnyFail;
}

TArray<FString> USqliteTool::GetDataTableHeader(FString DBPath, FString DataTableName)
{
	return GetDataTableHeaderForCpp(DBPath,DataTableName);
}

void USqliteTool::CollectData(FString KeyName, FString KeyValue, FString ColumnName, FString NewValue)
{
	FChangeDataInfo NewData(KeyName, KeyValue, ColumnName, NewValue);
	if (ChangedDatas.Contains(KeyValue))
	{
		ChangedDatas[KeyValue].Add(NewData);
	}
	else
	{
		TArray<FChangeDataInfo> NewDatas;
		NewDatas.Add(NewData);
		ChangedDatas.Add(KeyValue, NewDatas);
	}
}

void USqliteTool::SaveData(FString DBPath, FString DataTableName)
{
	if (SqliteConnect.Open(*DBPath, nullptr, nullptr))
	{
		for (auto ChangedData : ChangedDatas)
		{
			FString UpdateSql = "update " + DataTableName + " set ";
			FString KeyValue = ChangedData.Key;
			for (int i = 0; i < ChangedData.Value.Num(); ++i)
			{
				FChangeDataInfo ChangeDataInfo = ChangedData.Value[i];
				FString ColumnName = ChangeDataInfo.ColumnName;
				FString NewValue = ChangeDataInfo.NewValue;
				FString KeyName = ChangeDataInfo.KeyName;
				UpdateSql += ColumnName + "= \'" + NewValue + "\'";

				if (i < ChangedData.Value.Num() - 1)
				{
					UpdateSql += ",";
				}
				else
				{
					UpdateSql += " where " + KeyName + "=\'" + KeyValue + "\'";
				}
			}
			if (!SqliteConnect.Execute(*UpdateSql))
			{
				LogSqliteError(UpdateSql);
			}
		}
	}
	for (auto ChangedData : ChangedDatas)
	{
		ChangedData.Value.Empty();
	}
	ChangedDatas.Empty();
	SqliteConnect.Close();
	UE_LOG(LogTemp, Warning, TEXT("data save complete"));
}


void USqliteTool::LogSqliteError(FString SqlString)
{
	FString LastErrorStr = SqliteConnect.GetLastError();
	if (!LastErrorStr.Equals("not an error"))
	{
		UE_LOG(LogTemp, Warning, TEXT("sqlite execute failed: %s, sql string: %s"), *LastErrorStr, *SqlString);
	}
}
