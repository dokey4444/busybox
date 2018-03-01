package main

import "fmt"
import "reflect"

type inf interface {
	GetInt() int
	GetFloat() float32
	GetString() string
}

type obj struct {
	// Tag 空格分割，value必须使用双引号扩住
	i int     `type:"int" name:"i"`
	f float32 `type:"float32" name:"f"`
	s string  `type:"string" name:"s"`
	I int     `type:"int" name:"I"`
}

func (o obj) GetInt() int {
	return o.i
}

func (o obj) GetFloat() float32 {
	return o.f
}

func (o obj) GetString() string {
	return o.s
}

func main() {
	var st1 = obj{1, 2.3, "456", 0}
	var st2 obj
	var inf1 = st1
	// 获取指定对象的Type类型
	fmt.Println(reflect.TypeOf(st1))
	// 获取指定对象的Value对象
	fmt.Println(reflect.ValueOf(st1))
	// 将Value类型转换为对象类型
	st2 = reflect.ValueOf(st1).Interface().(obj)
	fmt.Println(st2)
	st2.i = 2
	fmt.Println(st1)
	fmt.Println(st2)
	// 获取指定Value对象的类型
	fmt.Println(reflect.ValueOf(st1).Type())
	// 获取指定Value对象的实际内置类型
	fmt.Println(reflect.ValueOf(st1).Kind())
	fmt.Println()

	var t1 = reflect.TypeOf(st1)
	// 遍历获取指定类型的所有字段名
	for i := 0; i < t1.NumField(); i++ {
		fmt.Println(t1.Field(i))
	}
	// 遍历获取指定类型的所有方法名
	var t2 = reflect.TypeOf(inf1)
	fmt.Println(t2.NumMethod())
	for i := 0; i < t2.NumMethod(); i++ {
		fmt.Println(t2.Method(i))
	}
	fmt.Println()

	// 指针与反射
	var pt1 = reflect.TypeOf(&st1)
	var pv1 = reflect.ValueOf(&st1)
	fmt.Println(pt1)
	fmt.Println(pv1)
	var ip = reflect.Indirect(pv1) // Indirect和Elem的区别在于，Elem对于非指针对象会异常
	fmt.Println(reflect.TypeOf(ip))
	fmt.Println(reflect.ValueOf(ip))
	fmt.Println()

	// 反射与赋值
	var i = 0
	var v reflect.Value
	v = reflect.ValueOf(&i)
	v = reflect.Indirect(v)
	fmt.Println(v.Interface())
	if v.Kind() == reflect.Int && v.CanSet() {
		v.SetInt(2)
	}
	fmt.Println(v.Interface().(int))

	v = reflect.ValueOf(&st1)
	v = reflect.Indirect(v)
	var f0 = v.FieldByName("I") // 只有公有变量才能修改
	fmt.Println(f0)
	fmt.Println(f0.Kind())
	fmt.Println(f0.CanSet())
	if f0.Kind() == reflect.Int && f0.CanSet() {
		f0.SetInt(2)
	}
	fmt.Println(f0)

	v = reflect.ValueOf(&st1)
	v = reflect.Indirect(v)
	f1, _ := v.Type().FieldByName("I")
	fmt.Println(f1.Tag.Get("name")) // 获取指定field的制定key
	for i := 0; i < v.NumField(); i++ {
		fmt.Println(v.Type().Field(i).Tag) // 所有field的tag，tag本质上以字符串存储
	}
}
