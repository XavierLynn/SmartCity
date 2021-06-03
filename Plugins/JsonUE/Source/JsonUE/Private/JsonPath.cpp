// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonPath.h"
#include "JsonUE.h"

void UJsonPath::Del(FString pathString) {
	if (pathString.Len()) Del(*pathString, json);
	else json.clear();
}
void UJsonPath::Add(FString pathString) {
	find(*pathString, json);
}
void UJsonPath::AddUInt(FString pathString, uint32 v) {
	find(*pathString, json) = v;
}
void UJsonPath::AddUInt64(FString pathString, uint64 v) {
	find(*pathString, json) = v;
}
void UJsonPath::AddInt(FString pathString, int v) {
	find(*pathString, json) = v;
}
void UJsonPath::AddInt64(FString pathString, int64 v) {
	find(*pathString, json) = v;
}
void UJsonPath::AddString(FString pathString, FString v) {
	find(*pathString, json) = v;
}
void UJsonPath::AddDouble(FString pathString, float v, uint8 precision) {
	double temp = 0.00; temp = v;// fangzhi gaowei de shuijishu 
	find(*pathString, json) = JsonUE::Value(temp, precision);
}

void UJsonPath::AddDoubleS(FString path, FString v, uint8 precision) {
	find(*path, json) = JsonUE::Value(JsonUE::FStringToDouble(v), precision);
}

void UJsonPath::AddBool(FString pathString, bool v) {
	find(*pathString, json) = v;
}

JsonUE::Value& UJsonPath::find(const TCHAR* begin, JsonUE::Value& root) const {
	//const TCHAR* Start = begin;
	JsonUE::Value* temp = nullptr;
	FString key;
	for (; begin &&*begin!='\0';++begin) {
		if (temp)return find(begin, *temp);// 已经找到
		switch (*begin) {
		case ']': temp = &root[FCString::Atoi(*key)]; break;
		case '}': temp = &root[key]; break;
		case '[':
		case '{':key.Empty(); break;
		case '\n':
		case '\r':
		case '\t':
		case ' ':break;
		default:key.AppendChar(*begin);
		}
	}
	return temp ? *temp : JsonUE::Value::nullSingleton();
}

bool UJsonPath::exist(const TCHAR* begin, JsonUE::Value& root) const {
	//const TCHAR* Start = begin;
	JsonUE::Value* temp = nullptr;
	FString key;
	for (; begin && *begin != '\0'; ++begin) {
		if (temp)return exist(begin, *temp);// 已经找到
		switch (*begin) {
		case ']': if (root.size() <= FCString::Atoi(*key))return false; temp = &root[FCString::Atoi(*key)]; break;
		case '}': if (root.find(key)==nullptr )return false; temp = &root[key]; break;
		case '[':
		case '{': key.Empty(); break;
		case '\n':
		case '\r':
		case '\t':
		case ' ':break;
		default:key.AppendChar(*begin);
		}
	}
	return temp ? true :false ;
}
bool UJsonPath::Del(const TCHAR* begin, JsonUE::Value& root) {
	FString key;
	for (; begin && *begin != '\0'; ++begin){
		switch (*begin) {
		case ']': {
			if (root.size() <= FCString::Atoi(*key))return true;
			if (!Del(begin + 1, root[FCString::Atoi(*key)]))root -= FCString::Atoi(*key);
			return true; 
			break;
		}
		case '}': {
			if (root.find(key) == nullptr)return true;
			if (!Del(begin + 1, root[key]))root -= key;
			return true;
			break;
		}
		case '[':
		case '{': key.Empty(); break;
		case '\n':
		case '\r':
		case '\t':
		case ' ':break;
		default:key.AppendChar(*begin);
		}
	}
	// 能到这里要么是key删除掉，要么是错误的格式，对于错误的格式不做后续删除
	return key.Len()?true:false;
}

FString UJsonPath::asString(FString path) const { return path.Len() == 0 ? json.asString() : find(*path, json).asString(); }
int UJsonPath::asInt(FString path) const { return path.Len() == 0 ? json.asInt64() : find(*path, json).asInt64(); }
int64 UJsonPath::asInt64(FString path) const { return path.Len() == 0 ? json.asInt64() : find(*path, json).asInt64(); }
uint64 UJsonPath::asUInt64(FString path) const { return path.Len() == 0 ? json.asUInt64() : find(*path, json).asUInt64(); }
float UJsonPath::asFloat(FString path) const { return path.Len() == 0 ? json.asFloat() : find(*path, json).asFloat(); }
double UJsonPath::asDouble(FString path) const { return path.Len() == 0 ? json.asDouble() : find(*path, json).asDouble(); }
bool UJsonPath::asBool(FString path) const { return path.Len() == 0 ? json.asBool() : find(*path, json).asBool(); }
bool UJsonPath::isNull(FString path) const { return path.Len() == 0 ? json.isNull() : find(*path, json).isNull(); }
bool UJsonPath::isBool(FString path) const { return path.Len() == 0 ? json.isBool() : find(*path, json).isBool(); }
bool UJsonPath::isInt64(FString path) const { return path.Len() == 0 ? json.isInt64() : find(*path, json).isInt64(); }
bool UJsonPath::isUInt64(FString path) const { return path.Len() == 0 ? json.isUInt64() : find(*path, json).isUInt64(); }
bool UJsonPath::isIntegral(FString path) const { return path.Len() == 0 ? json.isIntegral() : find(*path, json).isIntegral(); }
bool UJsonPath::isDouble(FString path) const { return path.Len() == 0 ? json.isDouble() : find(*path, json).isDouble(); }
bool UJsonPath::isNumeric(FString path) const { return path.Len() == 0 ? json.isNumeric() : find(*path, json).isNumeric(); }
bool UJsonPath::isString(FString path) const { return path.Len() == 0 ? json.isString() : find(*path, json).isString(); }
bool UJsonPath::isArray(FString path) const { return path.Len() == 0 ? json.isArray() : find(*path, json).isArray(); }
bool UJsonPath::isObject(FString path) const { return path.Len() == 0 ? json.isObject() : find(*path, json).isObject(); }
bool UJsonPath::isContain(FString path) const { return exist(*path, json); }
int32 UJsonPath::size(FString path) const { return path.Len()==0? json.size() : find(*path, json).size(); }

FString UJsonPath::ToString() const {
	JsonUE::Writer writer;
	if (isFormat)writer.setStyle(writer.defaultStyle());
	return writer.toString(json);
}

bool UJsonPath::FromString(FString content) {
	JsonUE::Reader reader;
	return reader.parse(content, json);
}

bool UJsonPath::ToFile(FString filePath) {
	return FFileHelper::SaveStringToFile(ToString(), *filePath);
}