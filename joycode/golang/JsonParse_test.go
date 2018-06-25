/*
// =====================================================================================
//
//       Filename:  JsonObj.go
//
//    Description:
//
//        Version:  1.0
//        Created:  06/23/2018 12:19:31 PM
//       Revision:  none
//       Compiler:  go1.10.3
//
//         Author:  boyi.gw, boyi.gw@alibaba-inc.com
//        Company:  Alibaba Group
//
// =====================================================================================
*/

package util

import (
	"fmt"
	"runtime"
	"strings"
	"testing"

	"inspector/util/unsafe"
)

var jsonData string = `
{
	"map": {
		"i" : 1024,
		"f" : 3.14,
		"bt" : true,
		"bf" : false,
		"n" : null,
		"o" : {
			"s1" : "s1",
			"s2" : "s2",
			"s3" : "s3",
			"s4" : "s4"
		},
		"l" : [1, 2, 3, 4, 5]
	},
	"list" : [
		1024, 3.14, true, false, null, {
			"s1" : "s1",
			"s2" : "s2",
			"s3" : "s3",
			"s4" : "s4"	
		},
		[1, 2, 3, 4, 5]
	]
}
`

var jsonIterLargeFile string = `
[{
	"person": {
		"id": "d50887ca-a6ce-4e59-b89f-14f0b5d03b03",
		"name": {
			"fullName": "Leonid Bugaev",
			"givenName": "Leonid",
			"familyName": "Bugaev"
		},
		"email": "leonsbox@gmail.com",
		"gender": "male",
		"location": "Saint Petersburg, Saint Petersburg, RU",
		"geo": {
			"city": "Saint Petersburg",
			"state": "Saint Petersburg",
			"country": "Russia",
			"lat": 59.9342802,
			"lng": 30.3350986
		},
		"bio": "Senior engineer at Granify.com",
		"site": "http://flickfaver.com",
		"avatar": "https://d1ts43dypk8bqh.cloudfront.net/v1/avatars/d50887ca-a6ce-4e59-b89f-14f0b5d03b03",
		"employment": {
			"name": "www.latera.ru",
			"title": "Software Engineer",
			"domain": "gmail.com"
		},
		"facebook": {
			"handle": "leonid.bugaev"
		},
		"github": {
			"handle": "buger",
			"id": 14009,
			"avatar": "https://avatars.githubusercontent.com/u/14009?v=3",
			"company": "Granify",
			"blog": "http://leonsbox.com",
			"followers": 95,
			"following": 10
		},
		"twitter": {
			"handle": "flickfaver",
			"id": 77004410,
			"bio": null,
			"followers": 2,
			"following": 1,
			"statuses": 5,
			"favorites": 0,
			"location": "",
			"site": "http://flickfaver.com",
			"avatar": null
		},
		"linkedin": {
			"handle": "in/leonidbugaev"
		},
		"googleplus": {
			"handle": null
		},
		"angellist": {
			"handle": "leonid-bugaev",
			"id": 61541,
			"bio": "Senior engineer at Granify.com",
			"blog": "http://buger.github.com",
			"site": "http://buger.github.com",
			"followers": 41,
			"avatar": "https://d1qb2nb5cznatu.cloudfront.net/users/61541-medium_jpg?1405474390"
		},
		"klout": {
			"handle": null,
			"score": null
		},
		"foursquare": {
			"handle": null
		},
		"aboutme": {
			"handle": "leonid.bugaev",
			"bio": null,
			"avatar": null
		},
		"gravatar": {
			"handle": "buger",
			"urls": [
			],
			"avatar": "http://1.gravatar.com/avatar/f7c8edd577d13b8930d5522f28123510",
			"avatars": [
			{
				"url": "http://1.gravatar.com/avatar/f7c8edd577d13b8930d5522f28123510",
				"type": "thumbnail"
			}
			]
		},
		"fuzzy": false
	},
	"company": "hello"
}]
`

func TestJsonParse(t *testing.T) {
	check := func(statement bool, msg string) {
		var line int
		_, _, line, _ = runtime.Caller(1)
		if statement == false {
			t.Error(fmt.Sprintf("line[%d] msg: %s\n", line, msg))
		}
	}
	var err error
	var buf strings.Builder
	var comma bool
	buf.WriteRune('{')
	err = JsonParse([]byte(jsonData), func(keyPath []string, value []byte, valueType JsonValueType) error {
		switch valueType {
		case STRING:
			if comma == true {
				buf.WriteRune(',')
			}
			if keyPath[len(keyPath)-1][0:1] != "[" {
				buf.WriteRune('"')
				buf.WriteString(keyPath[len(keyPath)-1])
				buf.WriteRune('"')
				buf.WriteRune(':')
			}
			buf.WriteRune('"')
			buf.Write(value)
			buf.WriteRune('"')
			comma = true
		case INTEGER:
			fallthrough
		case FLOAT:
			fallthrough
		case BOOL:
			fallthrough
		case NULL:
			if comma == true {
				buf.WriteRune(',')
			}
			if keyPath[len(keyPath)-1][0:1] != "[" {
				buf.WriteRune('"')
				buf.WriteString(keyPath[len(keyPath)-1])
				buf.WriteRune('"')
				buf.WriteRune(':')
			}
			buf.Write(value)
			comma = true
		case OBJECT:
			if value == nil { // OBJECT 开始
				if comma == true {
					buf.WriteRune(',')
				}
				if keyPath[len(keyPath)-1][0:1] != "[" {
					buf.WriteRune('"')
					buf.WriteString(keyPath[len(keyPath)-1])
					buf.WriteRune('"')
					buf.WriteRune(':')
				}
				buf.WriteRune('{')
				comma = false
			} else { // OBJECT 结束
				buf.WriteRune('}')
				comma = true
			}
		case ARRAY:
			if value == nil { // ARRAY 开始
				if comma == true {
					buf.WriteRune(',')
				}
				if keyPath[len(keyPath)-1][0:1] != "[" {
					buf.WriteRune('"')
					buf.WriteString(keyPath[len(keyPath)-1])
					buf.WriteRune('"')
					buf.WriteRune(':')
				}
				buf.WriteRune('[')
				comma = false
			} else { // ARRAY 结束
				buf.WriteRune(']')
				comma = true
			}
		}
		return nil
	})
	buf.WriteRune('}')
	jsonData = strings.Replace(jsonData, " ", "", -1)
	jsonData = strings.Replace(jsonData, "\t", "", -1)
	jsonData = strings.Replace(jsonData, "\n", "", -1)
	check(jsonData == buf.String(), "test")
	check(err == nil, "test")
	check(true, "test")
}

func TestJsonParseJsonIterLargeFile(t *testing.T) {
	check := func(statement bool, msg string) {
		var line int
		_, _, line, _ = runtime.Caller(1)
		if statement == false {
			t.Error(fmt.Sprintf("line[%d] msg: %s\n", line, msg))
		}
	}
	var err error
	var buf strings.Builder
	var comma bool
	buf.WriteRune('[')
	err = JsonParse([]byte(jsonIterLargeFile), func(keyPath []string, value []byte, valueType JsonValueType) error {
		switch valueType {
		case STRING:
			if comma == true {
				buf.WriteRune(',')
			}
			if keyPath[len(keyPath)-1][0:1] != "[" {
				buf.WriteRune('"')
				buf.WriteString(keyPath[len(keyPath)-1])
				buf.WriteRune('"')
				buf.WriteRune(':')
			}
			buf.WriteRune('"')
			buf.Write(value)
			buf.WriteRune('"')
			comma = true
		case INTEGER:
			fallthrough
		case FLOAT:
			fallthrough
		case BOOL:
			fallthrough
		case NULL:
			if comma == true {
				buf.WriteRune(',')
			}
			if keyPath[len(keyPath)-1][0:1] != "[" {
				buf.WriteRune('"')
				buf.WriteString(keyPath[len(keyPath)-1])
				buf.WriteRune('"')
				buf.WriteRune(':')
			}
			buf.Write(value)
			comma = true
		case OBJECT:
			if value == nil { // OBJECT 开始
				if comma == true {
					buf.WriteRune(',')
				}
				if keyPath[len(keyPath)-1][0:1] != "[" {
					buf.WriteRune('"')
					buf.WriteString(keyPath[len(keyPath)-1])
					buf.WriteRune('"')
					buf.WriteRune(':')
				}
				buf.WriteRune('{')
				comma = false
			} else { // OBJECT 结束
				buf.WriteRune('}')
				comma = true
			}
		case ARRAY:
			if value == nil { // ARRAY 开始
				if comma == true {
					buf.WriteRune(',')
				}
				if keyPath[len(keyPath)-1][0:1] != "[" {
					buf.WriteRune('"')
					buf.WriteString(keyPath[len(keyPath)-1])
					buf.WriteRune('"')
					buf.WriteRune(':')
				}
				buf.WriteRune('[')
				comma = false
			} else { // ARRAY 结束
				buf.WriteRune(']')
				comma = true
			}
		}
		return nil
	})
	buf.WriteRune(']')
	jsonIterLargeFile = strings.Replace(jsonIterLargeFile, " ", "", -1)
	jsonIterLargeFile = strings.Replace(jsonIterLargeFile, "\t", "", -1)
	jsonIterLargeFile = strings.Replace(jsonIterLargeFile, "\n", "", -1)
	checkString := buf.String()
	checkString = strings.Replace(checkString, " ", "", -1)
	checkString = strings.Replace(checkString, "\t", "", -1)
	checkString = strings.Replace(checkString, "\n", "", -1)
	check(jsonIterLargeFile == checkString, "test")
	check(err == nil, "test")
	check(true, "test")
}

func TestJsonParseInvalid(t *testing.T) {
	return
	check := func(statement bool, msg string) {
		var line int
		_, _, line, _ = runtime.Caller(1)
		if statement == false {
			t.Error(fmt.Sprintf("line[%d] msg: %s\n", line, msg))
		}
	}
	var err error

	err = JsonParse([]byte(` \t\n`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`a`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{ `), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{*`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{}{}`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{{}}`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{{},{}}`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a"`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a";`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":"1"`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":1`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":tru`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":trues`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":True`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":TRUE`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":fal`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":falses`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":False`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":FALSE`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":nul`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":nulls`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":Null`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":NULL`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":c`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":0,`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":0,}`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":0,"b":{`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`{"a":[]`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`[`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`[ `), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`["`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`["a`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`["a"`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`["a":`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`["a",`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`["a",{`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	err = JsonParse([]byte(`["a",{}`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err != nil, "test")

	check(true, "test")
}

func TestJsonParseValidEmpty(t *testing.T) {
	return
	check := func(statement bool, msg string) {
		var line int
		_, _, line, _ = runtime.Caller(1)
		if statement == false {
			t.Error(fmt.Sprintf("line[%d] msg: %s\n", line, msg))
		}
	}
	var err error

	err = JsonParse([]byte(`{}`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err == nil, "test")

	err = JsonParse([]byte(`{"a":[], "b":{}}`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err == nil, "test")

	err = JsonParse([]byte(`[]`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err == nil, "test")

	err = JsonParse([]byte(`[[], [] ,[]]`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err == nil, "test")

	err = JsonParse([]byte(`[{}, [], {}]`), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	check(err == nil, "test")

	check(true, "test")
}

func BenchmarkJsonParse(b *testing.B) {
	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		JsonParse(unsafe.String2Bytes(jsonData), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	}
}

func BenchmarkJsonParseJsonIterLargeFile(b *testing.B) {
	b.ResetTimer()
	b.ReportAllocs()

	for i := 0; i < b.N; i++ {
		JsonParse(unsafe.String2Bytes(jsonIterLargeFile), func(keyPath []string, value []byte, valueType JsonValueType) error { return nil })
	}
}
