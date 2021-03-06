package com.furture.react;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.Map.Entry;

/**
 * DuktapeEngine 支持多线程，但单个实例同时仅支持一个线程执行，多个线程交替串行执行。
 * 如果需要完全并发，建议利用Java层实现并发，然后把并发结果通过Handler转交给DuktapeEngine。
 * 或者在调用的地方用synchronized (engine){
 *
 * } 控制线程并发，但请注意避免死锁。
 * */
public class DuktapeEngine {

	static{
		try {
			System.loadLibrary("DuktapeEngine");
			soLoaded = true;
		} catch (Throwable e) {
			soLoaded = false;
		}
	}

	/**
	 * 某些环境so会失败
	 * */
	private static boolean soLoaded;

	/**
	 * 引擎的JNI指针地址
	 * */
	private long  ptr;

	/**
	 * 引擎的中JSRef批量回收队列，以及批量回收触发阀值的大小。
	 * */
	private List<Integer> finalizedJSRefList;
	private static  final int FINALIZE_SIZE = 8;



    /**
	 * 创建DuktapeEngine
	 * 引擎使用完成后，调用destory()销毁引擎
	 * */
	public DuktapeEngine() {
		if (!soLoaded) {
			ptr = 0;
			return;
		}
		ptr = nativeInit();
		if (ptr == 0) {
			throw new RuntimeException("NativeInit Pointer Convert Error");
		}
		finalizedJSRefList = new ArrayList<Integer>(FINALIZE_SIZE*2);
		Set<Entry<String, Object>> entries = JSContext.getContext().entrySet();
		for(Entry<String, Object> entry : entries){
			put(entry.getKey(), entry.getValue());
		}
	}


	/**
	 * @param  script
	 * @return  执行JavaScript返回执行结果
	 * */
	public Object execute(String script){
		if(ptr != 0){
			return nativeExeclute(ptr, script);
		}else{
			return null;
		}
	}

	/**
	 * @param  key  导入java对象在javascript中的名字
	 * @param  value Java对象
     * <br>
	 * 导入Java对象到JavaScript引擎中供JavaScript调用；此方法可用于初始化一些公共的对象。
	 * */
	public void put(String key, Object value){
		if(ptr != 0){
			nativeRegister(ptr, key, value);
		}
	}

	/**
	 * 获取JavaScript的公共的对象。
	 */
	public  Object get(String propName){
		if(ptr != 0){
			return nativeGet(ptr, propName);
		}
		return  null;
	}

	/**
	 * @param jsRef        javascript对象
	 * @param methodName   js对象的方法名
	 * @param args         方法参数
	 *
	 *
	 * @return 调用异常返回null，异常信息输出到logcat中
     * <br>
     * 若javascript对象为function，忽略methodName，直接调用该function。对于直接function的调用，methodName可以传null
	 * 若javascript对象为object，则调用object中的methodName对应的function方法或者属性。
	 * 这样在javascript 通过两种方式书写回调。 如要写一个setOnClickListener(new OnClickListener())的回调。
	 * 在javascript中可以通过一下两种方式书写：
	 * view.setOnClickListener(new OnClickListener({
	 *    onClick : function(view){
	 *
	 *     }
	 * }));
	 *
	 * 或者采用更为简便的方式书写：
	 *
	 * view.setOnClickListener(new OnClickListener(function(view){
	 *
	 * }));
	 *
     * */
    public   Object call(JSRef jsRef, String methodName, Object... args){
			if(ptr != 0){
				return nativeCallJSRef(ptr, jsRef.getRef(),  methodName, args);
			}else{
				return null;
			}
	}


	/**
	 * 获取JavaScript的对象的属性的值。
	 * var value  = jsRef.propName
	 */
	public   Object get(JSRef jsRef, String propName){
		if(ptr != 0){
			return nativeGetJSRef(ptr, jsRef.getRef(), propName);
		}
		return null;
	}

	/**
	 * 设置JavaScript的对象的属性的值。
	 * jsRef.propName = propValue
	 */
	public   void set(JSRef jsRef, String propName, Object propValue){
		if(ptr != 0){
			nativeSetJSRef(ptr, jsRef.getRef(), propName, propValue);
		}
	}


	/**
	 * @param objectName   js对象名字
	 * @param method       js对象的方法名
	 * @param args         方法参数
     * @return   调用异常返回null, 正常返回调用结果
     * <br>
	 * 直接调用js中的方法, 调用异常返回null，异常信息输出到logcat中
	 * */
    public  Object call(String objectName, String method, Object... args){
		if(ptr != 0){
			return nativeCallJs(ptr, objectName, method, args);
		}else {
			return null;
		}
	}




	/**
	 * 销毁引擎，释放引擎对应的native资源；引擎销毁后，调用不在生效。
	 * */
	public void destory(){
		synchronized (this) {
			if (ptr != 0) {
				nativeDestory(ptr);
				ptr = 0;
			}
		}
	}

	/**
	 * 释放除了重用的JSRef之外的无用JSRef引用
	 * @param  resuseRef 重用的JSRef
	 * */
	void releaseFinalizedJSRefs(int resuseRef){
		if (finalizedJSRefList.size() <= FINALIZE_SIZE){
			return;
		}
		synchronized (finalizedJSRefList){
			synchronized (this){
				Iterator<Integer>  it =  finalizedJSRefList.iterator();
				while (it.hasNext()){
					if (ptr != 0) {
						int ref = it.next();
						if(ref == resuseRef){
							continue;
						}
						nativeFinalizeJSRef(ptr, ref);
					}
					it.remove();
				}
			}
		}
	}

	/**
	 * 放到finalizedJSRefList中，然后在Duktape线程中批量回收。
	 * 不阻塞finalize java内存回收的进程，当道Duktape执行的线程中回收，
	 * 防止并发回收。
	 * */
   void finalizeJSRef(final int jsRef){
	   synchronized (finalizedJSRefList){
		   finalizedJSRefList.add(jsRef);
	   }
	}

    /**
	 * JNI函数接口
	 * */
	private  native long nativeInit();
	private  native void nativeRegister(long ptr, String key, Object value);
	private  native Object nativeGet(long ptr, String key);
	private  native Object nativeExeclute(long ptr, String script);
	private  native Object nativeCallJs(long ptr, String target, String method, Object... args);
	private  native Object nativeCallJSRef(long ptr, int jsRef, String methodName, Object... args);
	private  native Object nativeGetJSRef(long ptr, int jsRef, String propName);
	private  native void nativeSetJSRef(long ptr, int jsRef, String propName, Object value);
	private native void nativeFinalizeJSRef(long ptr, int jsRef);
	private native void nativeDestory(long ptr);
}
