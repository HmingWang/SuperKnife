package com.whaim.superknife.common;

import lombok.Data;

@Data
public class ApiResult<T> {

    private final static String SUCCESS="00";
    private final static String FAILED="99";

    private  boolean isSuccess;
    private  String code;
    private  String message;
    private T data;

    public ApiResult(boolean b,String c,String m,T o){
        isSuccess=b;
        code=c;
        message=m;
        data=o;
    }

    public static<T> ApiResult<T> success(T o){
        return new ApiResult<>(true, SUCCESS, "处理成功", o);
    }

    public static<T> ApiResult<T> success(){
        return new ApiResult<>(true,SUCCESS,"处理成功",null);
    }

    public static<T> ApiResult<T> failed(){
        return new ApiResult<>(false,FAILED,"处理失败",null);
    }

    public static<T> ApiResult<T> failed(String m){
        return new ApiResult<>(false,FAILED,m,null);
    }

    public static<T> ApiResult<T> failed(String m,T o){
        return new ApiResult<>(false,FAILED,m,o);
    }
}
