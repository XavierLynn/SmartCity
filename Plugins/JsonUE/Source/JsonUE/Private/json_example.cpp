#include "jsonue/jsonue.h"

/*
{"school":"���������ѧ",
 "address":"����������������·51��(���ƻ�԰����)",
 "point":{"long":104.363505,"lat":36.12345678},
 "KeySchools":true,
 "teachers":null,
 "student":[{"name":"����","age":12},{"name":"lishi","age":15}]
}
*/
JsonUE::Value construct_school() {
	JsonUE::Value json;// this is json root node
	json["school"] = TEXT("���������ѧ");
	json["address"] = TEXT("����������������·51��(���ƻ�԰����)");
	json["point"]["long"] = 104.363505;// Ĭ�ϸ��㾫��С�����4λ
	json["point"]["lat"] = JsonUE::Value(36.12345678, 8);
	json["KeySchools"] = true;// 
	json["teachers"] ;// ����һ���յ� json �ڵ�
	json["student"][0]["name"] = TEXT("����");
	json["student"][0]["age"] = 12;
	json["student"][1]["name"] = TEXT("lishi");
	json["student"][1]["age"] = 15;
	JsonUE::Writer writer;
	// writer.setStyle(writer.defaultStyle()); // if set need please use this code;
	FString content = writer.toString(json);
	return json;
}

JsonUE::Value construct_json() {
	JsonUE::Value json;// this is json root node
	json["key_string"] = TEXT("���Ĳ�����");
	json["key_int"] = 123;
	json["key_bool"] = true;
	json["key_null"] ;// ����һ���յ� json �ڵ�
	json["key_double"] = 0.4321;// Ĭ�ϸ��㾫��С�����4λ
	json["key_double_precision"] = JsonUE::Value(0.98231241435,6);// ָ���������ľ���
	json["key_key"]["key"] = "this is sub object";// json����Ƕ��json
	json["key_array"][0] = "one";// json Ƕ������
	json["key_array"][1] = "two";
	json["key_array"][5] = "three and four is null,five is not null ";
	json["key_array_key"][0]["array_key"] = "^_^"; // ����Ƕ��json
	json["key_array_array"][0][0] = "$\"$"; // ����Ƕ������
	json["key_delete"] = "delete";
	return json;
}

void deleteNode(JsonUE::Value& json ) {
	json["key_array"] -= 0;// delete first node in array
	json -= "key_delete";// delete key
}

bool checkValue(JsonUE::Value& json) {
	if (!json.isNull())return false;// check json is null
	if (!json.isObject())return false;// type of root is not json object
	if (!json.find("key_int"))return false;// key_int is not exist in root of json
	if (!json["key_int"].isInt64())return false;// type of key_int is not int64
	if (!json["key_bool"].isBool())return false;// type of key_bool is not bool 
	if (!json["key_double"].isDouble())return false;// type of key_bool is not double
	if (!json["key_array"].isArray())return false;// type of key_bool is not array
	return true;
}

void getValue(JsonUE::Value& json) {
	int64 key_int = json["key_int"].asInt64();
	bool key_bool = json["key_bool"].asBool();
	FString key_string = json["key_string"].asString();
	double key_double = json["key_double"].asDouble();
}

FString jsonToString(JsonUE::Value& json) {
	JsonUE::Writer writer;
	// writer.setStyle(writer.defaultStyle()); // if set need please use this code;
	FString content = writer.toString(json);
	return content;
}

JsonUE::Value stringToJson(FString content) {
	JsonUE::Value parse;
	JsonUE::Reader reader;
	if (reader.parse(content, parse)) {
		// this is succed
	}
	else {
		// this is failed.
	}
	return parse;
}









