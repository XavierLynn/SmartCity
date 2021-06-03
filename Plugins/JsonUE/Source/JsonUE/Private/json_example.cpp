#include "jsonue/jsonue.h"

/*
{"school":"重庆巴蜀中学",
 "address":"重庆市渝中区北区路51号(近黄花园大桥)",
 "point":{"long":104.363505,"lat":36.12345678},
 "KeySchools":true,
 "teachers":null,
 "student":[{"name":"张三","age":12},{"name":"lishi","age":15}]
}
*/
JsonUE::Value construct_school() {
	JsonUE::Value json;// this is json root node
	json["school"] = TEXT("重庆巴蜀中学");
	json["address"] = TEXT("重庆市渝中区北区路51号(近黄花园大桥)");
	json["point"]["long"] = 104.363505;// 默认浮点精度小数点后4位
	json["point"]["lat"] = JsonUE::Value(36.12345678, 8);
	json["KeySchools"] = true;// 
	json["teachers"] ;// 产生一个空的 json 节点
	json["student"][0]["name"] = TEXT("张三");
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
	json["key_string"] = TEXT("中文不乱码");
	json["key_int"] = 123;
	json["key_bool"] = true;
	json["key_null"] ;// 产生一个空的 json 节点
	json["key_double"] = 0.4321;// 默认浮点精度小数点后4位
	json["key_double_precision"] = JsonUE::Value(0.98231241435,6);// 指定浮点数的精度
	json["key_key"]["key"] = "this is sub object";// json对象嵌套json
	json["key_array"][0] = "one";// json 嵌套数组
	json["key_array"][1] = "two";
	json["key_array"][5] = "three and four is null,five is not null ";
	json["key_array_key"][0]["array_key"] = "^_^"; // 数组嵌套json
	json["key_array_array"][0][0] = "$\"$"; // 数组嵌套数组
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









