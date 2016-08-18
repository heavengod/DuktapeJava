/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include "DuktapeEngine.h"

#define convert_to_context(ptr)    ((duk_context *)((intptr_t)ptr))

#define convert_to_ptr(ctx)    ((jlong)((intptr_t)ctx))

#define convert_ptr_to_long(ctx)    ((jlong)((intptr_t)ctx))

#define convert_long_to_ptr(ctx)   ((void *)((intptr_t)ptr))

#define  JAVA_OBJECT_MARK    "__j"

#define  JS_REF_MARK    "__jr"

#define  JAVA_METHOD_MARK    "__c"

#define  JAVA_GET_MARK    "__g"

#define  JAVA_SET_MARK    "__s"

#define JAVA_CLASS_NAME_KEY "__cls"

#define  JAVA_ENGINE_MARK    "__engine"

static jclass java_api_class = NULL;

static jmethodID java_function_method = NULL;

static jmethodID java_field_get_method = NULL;

static jmethodID java_import_class_method = NULL;

static jmethodID java_field_set_method = NULL;

static jmethodID java_new_instance_method = NULL;

static jmethodID java_exception_get_stack_trace_method = NULL;

static jclass js_ref_class = NULL;
static jmethodID js_ref_new_method = NULL;
static jmethodID js_ref_get_ref_method = NULL;

static jclass java_boolean_class  = NULL;
static jclass java_integer_class  = NULL;
static jclass java_long_class  = NULL;
static jclass java_double_class  = NULL;
static jclass java_float_class  = NULL;
static jclass java_short_class  = NULL;
static jclass java_byte_class  = NULL;
static jclass java_number_class  = NULL;
static jclass java_lang_object_class  = NULL;
static jmethodID java_number_get_double_value_method  = NULL;
static jmethodID java_boolean_get_boolean_value_method  = NULL;
static jmethodID java_boolean_constructor_method = NULL;
static jmethodID java_double_constructor_method = NULL;
static jmethodID java_lang_to_string_method = NULL;


static JavaVM*  jvm;
static pthread_key_t jni_env_key;


void duk_push_java_object(duk_context *ctx, JNIEnv *env, jobject object);

int duk_push_java_object_array(duk_context *ctx, JNIEnv *env, jobjectArray args);

int  duk_new_java_class(duk_context *ctx);

void duk_mark_jsobject_to_java_object(duk_context *ctx, int index, JNIEnv *env, jobject object);

jobjectArray duk_to_java_object_array(duk_context *ctx, int start, int num, JNIEnv*  env);

const char * duk_js_error_to_string(duk_context *ctx, int index);

JNIEnv*  get_java_jni_env();


jobject get_engine_from_context(duk_context *ctx){
	  duk_push_global_object(ctx);
		if(duk_get_prop_string(ctx, -1, JAVA_ENGINE_MARK)){
			  jobject  engine =  duk_to_pointer(ctx, -1);
		    duk_pop_2(ctx);
		    return engine;
		}
		duk_pop_2(ctx);
		return NULL;
}

const char * duk_js_error_to_string(duk_context *ctx, int index){
	if(!duk_is_error(ctx, index)){
		   DEBUG_LOG("ScriptEngine","duk_js_error_to_string is not error object");
	    return duk_js_error_to_string(ctx, index);
	}

	if(duk_get_prop_string(ctx, index, "stack")){
		 DEBUG_LOG("ScriptEngine","duk_js_error_to_string stack");
		 duk_get_prop_string(ctx, index - 1, "lineNumber");
		 int lineNumber = duk_get_int(ctx, -1);
		 duk_pop(ctx);
		 duk_push_sprintf(ctx, "javascript source %d line %s", lineNumber, duk_safe_to_string(ctx, -1));
		 duk_replace(ctx, index-2);
		 duk_pop(ctx);
		 return duk_safe_to_string(ctx, -1);
	}
	duk_pop(ctx);

	if(duk_get_prop_string(ctx, index, "lineNumber")){
         int lineNumber = duk_get_int(ctx, -1);
         duk_pop(ctx);
         duk_push_sprintf(ctx, " %s  at javascript source %d line", duk_to_string(ctx, index), lineNumber);
         if(index < 0){
            duk_replace(ctx, index - 1);
         }else{
        	  duk_replace(ctx, index);
         }
         return duk_to_string(ctx,index);
	}
	duk_pop(ctx);
	return duk_to_string(ctx, index);
}

jobject duk_to_java_object(duk_context *ctx, JNIEnv*  env, int i){
	int type = duk_get_type(ctx, i);
	if(type == DUK_TYPE_OBJECT){
		  if(duk_get_prop_string(ctx, i, JAVA_OBJECT_MARK)){
			    DEBUG_LOG("ScriptEngine","invoke_java_method call, convert %d args to java object", i);
		  		jobject  value = duk_to_pointer(ctx, -1);
					if(value != NULL){
			  	    value = (*env)->NewLocalRef(env, value);
					}
					duk_pop(ctx);
					return value;
		  }else{
			  duk_pop(ctx);
			  if(duk_get_prop_string(ctx, i, JS_REF_MARK)){
				  DEBUG_LOG("ScriptEngine","reuse javascript object's JSRef");
				  jweak  weakRef = duk_to_pointer(ctx, -1);
				  jobject jsRefObject = (*env)->NewLocalRef(env, weakRef);
				  duk_pop(ctx);
				  if(jsRefObject != NULL){
					  return jsRefObject;
				  }
			  }else{
				  duk_pop(ctx);
			  }
			  duk_dup(ctx, i);
			  jint ref = duk_js_ref(ctx);
			  if(ref != 0){
				  DEBUG_LOG("ScriptEngine","convert javascript object to JSRef Ref Value %d ", ref);
				  jobject engine = get_engine_from_context(ctx);
				  jobject jsRefObject = (*env)->NewObject(env, js_ref_class, js_ref_new_method, engine, ref);
				  if(jsRefObject != NULL){
					  jweak jsWeakRef = (*env)->NewWeakGlobalRef(env, jsRefObject);
					  if(jsWeakRef != NULL){
                         duk_dup(ctx, i);
                         duk_push_pointer(ctx, jsWeakRef);
                         duk_put_prop_string(ctx, -2, JS_REF_MARK);
                         duk_pop(ctx);
                         return jsRefObject;
					  }else{
							 LOGE("ScriptEngine","ScriptEngine failed make JSRef weak reference  on low memory jvm, please release some memory");
					  }
				 }
			  }
			  return NULL;
		  }
	}else if(type == DUK_TYPE_STRING){
			 const char*  chs = duk_to_string(ctx, i);
			 DEBUG_LOG("ScriptEngine","invoke_java_method call, convert %d args to string %s", i, chs);
			return (*env)->NewStringUTF(env, chs);
 }else if(type == DUK_TYPE_BOOLEAN){
			jboolean value = duk_to_boolean(ctx, i);
			DEBUG_LOG("ScriptEngine","invoke_java_method call, convert %d args to boolean %d", i, value);
			jobject booleanObject = (*env)->NewObject(env, java_boolean_class, java_boolean_constructor_method, value);
			return  booleanObject;
	}else if(type == DUK_TYPE_NUMBER){
			jdouble value = duk_to_number(ctx, i);
			jobject numberObject = (*env)->NewObject(env, java_double_class, java_boolean_constructor_method, value);
			DEBUG_LOG("ScriptEngine","invoke_java_method call, convert %d args to number %f", i, value);
			return numberObject;
	 }else if(type == DUK_TYPE_NULL || type == DUK_TYPE_UNDEFINED){
 			return NULL;
 	 }
	 LOGW("ScriptEngine","ScriptEngine duk_to_java_object call, unhandled type convert %d args %s to null", i, duk_to_string(ctx, i));
	 return NULL;
}



int invoke_java_method_call(duk_context *ctx) {
  int num = duk_get_top(ctx);
	const char * method = duk_to_string(ctx, 0);
	duk_push_this(ctx);
	if(duk_get_prop_string(ctx, -1, JAVA_OBJECT_MARK)){
		  jobject  ref = duk_to_pointer(ctx, -1);
		  JNIEnv*  env =  get_java_jni_env();
		  jstring methodName = (*env)->NewStringUTF(env, method);
	    jobjectArray args = duk_to_java_object_array(ctx, 1, num-1, env);
		  jobject value =  (*env)->CallStaticObjectMethod(env, java_api_class, java_function_method, ref, methodName, args);
      if((*env)->ExceptionCheck(env)){
					jthrowable exp = ( *env)->ExceptionOccurred(env);
					 if(exp != NULL){
										( *env)->ExceptionClear(env);
										jstring exceptionMessage = (*env)->CallStaticObjectMethod(env, java_api_class, java_exception_get_stack_trace_method, exp);
									 jboolean isCopy = JNI_FALSE;
										const char* cstrMessage = (*env)->GetStringUTFChars(env, exceptionMessage, &isCopy);
										duk_push_error_object(ctx, DUK_ERR_EVAL_ERROR | 0x01000000L , "invoke java method  %s exception \n %s",  method, cstrMessage);
										(*env)->ReleaseStringUTFChars(env, exceptionMessage, cstrMessage);
										( *env)->DeleteLocalRef(env , exceptionMessage);
										(*env)->DeleteLocalRef(env, args);
										(*env)->DeleteLocalRef(env, methodName);
										duk_throw(ctx);
										return 0;
					 }
			}
		  duk_push_java_object(ctx, env, value);
	    (*env)->DeleteLocalRef(env, value);
	    (*env)->DeleteLocalRef(env, args);
	    (*env)->DeleteLocalRef(env, methodName);
	    DEBUG_LOG("ScriptEngine","invoke_java_method call method %s success", method);
		  return 1;
	}else{
		duk_pop(ctx);
		duk_insert(ctx, 0);
		DEBUG_LOG("ScriptEngine","invoke_script_prop call, with args  num %d ", duk_get_top(ctx));
		if(duk_pcall_prop(ctx, 0, num - 1) != DUK_EXEC_SUCCESS){
			LOGE("ScriptEngine","ScriptEngine call %s method %s error %s", duk_to_string(ctx, 0), method, duk_js_error_to_string(ctx, -1));
			duk_pop(ctx);
			duk_push_null(ctx);
		}
		return 1;
	}
}




static int java_object_finalizer(duk_context *ctx) {
	int n = duk_get_top(ctx);
	if(duk_get_prop_string(ctx, 0, JAVA_OBJECT_MARK)){
		duk_del_prop_string(ctx, 0, JAVA_OBJECT_MARK);
		jobject  ref = duk_to_pointer(ctx, -1);
		if(ref != NULL){
				DEBUG_LOG("ScriptEngine",  "java_object_finalizer %p ", ref);
				JNIEnv *env = get_java_jni_env();
				if(env){
					(*env)->DeleteGlobalRef(env, ref);
				}
		}
	}
	return 0;
}


static int java_object_to_string(duk_context *ctx) {
	duk_push_this(ctx);
	if(duk_get_prop_string(ctx, -1, JAVA_OBJECT_MARK)){
		jobject  ref = duk_to_pointer(ctx, -1);
		if(ref == NULL){
			 duk_push_null(ctx);
			 return 1;
		}
		JNIEnv*  env =  get_java_jni_env();
		jstring  value =  (*env)->CallObjectMethod(env, ref, java_lang_to_string_method);
		jboolean iscopy = JNI_FALSE;
		const char* src =  ((*env)->GetStringUTFChars(env, value, &iscopy));
		DEBUG_LOG("ScriptEngine",  "java_object_to_string success %p  %s", ref, src );
    duk_push_string(ctx, src);
    (*env)->ReleaseStringUTFChars(env, value, src);
		(*env)->DeleteLocalRef(env, value);
		return 1;
	}
	duk_pop(ctx);
	duk_to_string(ctx, -1);
	return 1;
}

static duk_ret_t duk_java_property_get(duk_context *ctx) {
	const char* key  = duk_to_string(ctx, 0);
	duk_push_this(ctx);
	if(duk_get_prop_string(ctx, -1, JAVA_OBJECT_MARK)){
		jobject  ref = duk_to_pointer(ctx, -1);
		JNIEnv*  env =  get_java_jni_env();
		jstring fieldName = (*env)->NewStringUTF(env, key);
		jobject  value =  (*env)->CallStaticObjectMethod(env, java_api_class, java_field_get_method, ref, fieldName);
		if((*env)->ExceptionCheck(env)){
			 jthrowable exp = ( *env)->ExceptionOccurred(env);
			 if(exp != NULL){
						 ( *env)->ExceptionClear(env);
						 jstring exceptionMessage = (*env)->CallStaticObjectMethod(env, java_api_class, java_exception_get_stack_trace_method, exp);
							 jboolean isCopy = JNI_FALSE;
						 const char* cstrMessage = (*env)->GetStringUTFChars(env, exceptionMessage, &isCopy);
						 duk_push_error_object(ctx, DUK_ERR_EVAL_ERROR, "get java property %s error \n %s",  key, cstrMessage);
						 (*env)->ReleaseStringUTFChars(env, exceptionMessage, cstrMessage);
						 ( *env)->DeleteLocalRef(env , exceptionMessage);
						 (*env)->DeleteLocalRef(env, value);
						 (*env)->DeleteLocalRef(env, fieldName);
						 duk_throw(ctx);
						 return 0;
				}
		}
		DEBUG_LOG("ScriptEngine", "duk_java_property_get key %s %p", key, value);
		duk_push_java_object(ctx, env, value);
		(*env)->DeleteLocalRef(env, value);
		(*env)->DeleteLocalRef(env, fieldName);
		return 1;
	}else{
		duk_get_prop_string(ctx, 1, key);
		if(duk_is_undefined(ctx, -1)){
			LOGW("ScriptEngine", "ScriptEngine warn property  %s not found ", key);
		}
		return 1;
	}
}

static duk_ret_t duk_java_property_set(duk_context *ctx) {
	 const char* key  = duk_to_string(ctx, 0);
	 DEBUG_LOG("ScriptEngine", "duk_java_property_set  key %s ", key);
	 duk_push_this(ctx);
     if(duk_get_prop_string(ctx, -1, JAVA_OBJECT_MARK)){
			jobject  ref = duk_to_pointer(ctx, -1);
			JNIEnv*  env =  get_java_jni_env();
			jstring fieldName = (*env)->NewStringUTF(env, key);
			jobject  fieldValue = duk_to_java_object(ctx, env, 1);
			(*env)->CallStaticVoidMethod(env, java_api_class, java_field_set_method, ref, fieldName, fieldValue);
			DEBUG_LOG("ScriptEngine", "duk_java_property_set  call staticVoidMethod %s  success", key);
			jthrowable exp = (*env)->ExceptionOccurred(env);
			if(exp != NULL){
					  (*env)->ExceptionClear(env);
					  jstring exceptionMessage = (*env)->CallStaticObjectMethod(env, java_api_class, java_exception_get_stack_trace_method, exp);
				      jboolean isCopy = JNI_FALSE;
					  const char* cstrMessage = (*env)->GetStringUTFChars(env, exceptionMessage, &isCopy);
					  duk_push_error_object(ctx, DUK_ERR_EVAL_ERROR, "get java property %s error \n %s",  key, cstrMessage);
					  (*env)->ReleaseStringUTFChars(env, exceptionMessage, cstrMessage);
					  ( *env)->DeleteLocalRef(env , exceptionMessage);
					  (*env)->DeleteLocalRef(env, fieldValue);
					  (*env)->DeleteLocalRef(env, fieldName);
					  duk_throw(ctx);
					  return 0;
			 }
			(*env)->DeleteLocalRef(env, fieldValue);
			(*env)->DeleteLocalRef(env, fieldName);
			return 0;
	 }else{
		  duk_pop(ctx);
		  duk_replace(ctx, 0);
	    duk_put_prop_string(ctx, 0, key);
	    DEBUG_LOG("ScriptEngine", "duk_java_property_set  key %s ", key);
		  return 0;
	 }
}


void duk_push_java_object(duk_context *ctx, JNIEnv *env, jobject object){
	if(object == NULL){
		duk_push_null(ctx);
		return;
	}
	if((*env)->IsInstanceOf(env, object, js_ref_class)){
		jint  ref = (*env)->CallIntMethod(env, object, js_ref_get_ref_method);
		duk_push_js_ref(ctx, ref);
		return;
	}

	if((*env)->IsInstanceOf(env, object, java_number_class)){
			jdouble num = (*env)->CallDoubleMethod(env, object, java_number_get_double_value_method);
			duk_push_number(ctx, num);
	    return;
	}
	if((*env)->IsInstanceOf(env, object, java_boolean_class)){
		jboolean value = (*env)->CallBooleanMethod(env, object, java_boolean_get_boolean_value_method);
		if(value){
			duk_push_true(ctx);
		}else{
			duk_push_false(ctx);
		}
		return;
	}
	duk_push_object(ctx);  //empty target, java to js ret map, cache map
	duk_mark_jsobject_to_java_object(ctx, -1, env, object);
}

/**
*   https://www.ibm.com/developerworks/library/j-jni/
*/
int duk_push_java_object_array(duk_context *ctx, JNIEnv *env, jobjectArray args){
	  jsize length =  0;
	  if(args != NULL){
		 length = (*env)->GetArrayLength(env, args);
	  }
	  DEBUG_LOG("ScriptEngine","duk_push_java_object_array length %d", length);
	  for(int i=0; i<length; i++){
		  jobject object =  (*env)->GetObjectArrayElement(env, args, i);
		  duk_push_java_object(ctx, env, object);
	  }
	  return length;
}



void duk_mark_jsobject_to_java_object(duk_context *ctx, int index, JNIEnv *env, jobject object){
	jobject  ref = ((*env)->NewGlobalRef(env, object));
	duk_push_pointer(ctx, ref);
  duk_put_prop_string(ctx, index - 1, JAVA_OBJECT_MARK);
	duk_push_c_function(ctx, java_object_to_string, 0);
	duk_put_prop_string(ctx, index - 1, "toString");
	duk_push_c_function(ctx, java_object_finalizer, 1);
	duk_set_finalizer(ctx, index - 1);
}





int duk_import_java_class(duk_context *ctx){
		int n = duk_get_top(ctx);
		const char* className = duk_to_string(ctx, 0);
		const char*  shortName = NULL;
		if(n > 1){
			shortName = duk_to_string(ctx, 1);
		}else{
			shortName = strrchr(className, '.');
			shortName++;
		}
		DEBUG_LOG("DuktapeEngine", "duk_import_java_class className %s  shorName %s",className, shortName);


		JNIEnv *env = get_java_jni_env();
		jstring  fullClassName = (*env)->NewStringUTF(env, className);
	  jclass   importClass =  (*env)->CallStaticObjectMethod(env, java_api_class, java_import_class_method, fullClassName);
			if((*env)->ExceptionCheck(env)){
	 			 jthrowable exp = ( *env)->ExceptionOccurred(env);
	 			 if(exp != NULL){
	 								 ( *env)->ExceptionClear(env);
	 								 jstring exceptionMessage = (*env)->CallStaticObjectMethod(env, java_api_class, java_exception_get_stack_trace_method, exp);
	 								 jboolean isCopy = JNI_FALSE;
	 								 const char* cstrMessage = (*env)->GetStringUTFChars(env, exceptionMessage, &isCopy);
	 								 duk_push_error_object(ctx, DUK_ERR_EVAL_ERROR, "importClass exception \n %s", cstrMessage);
	 								 (*env)->DeleteLocalRef(env, fullClassName);
	 								 duk_throw(ctx);
	 								 return 0;
	 			 }
	 	 }
		duk_push_global_object(ctx);
		duk_push_c_function(ctx, duk_new_java_class,  DUK_VARARGS);
		duk_mark_jsobject_to_java_object(ctx, -1, env, importClass);
	  (*env)->DeleteLocalRef(env, importClass);
		(*env)->DeleteLocalRef(env, fullClassName);
		duk_push_object(ctx);
	  duk_push_string(ctx, className);
    duk_put_prop_string(ctx, -2, JAVA_CLASS_NAME_KEY);
    duk_put_prop_string(ctx, -2, "prototype");
    duk_put_prop_string(ctx, -2, shortName);
    duk_pop(ctx);
    return 0;
}

jobjectArray duk_to_java_object_array(duk_context *ctx, int start, int num, JNIEnv*  env){
	 jobjectArray args = (*env)->NewObjectArray(env, num, java_lang_object_class, NULL);
	 for(int i=0; i< num; i++){
	    	  jobject   value = duk_to_java_object(ctx, env, i + start);
	    	  DEBUG_LOG("ScriptEngine","duk_to_java_object_array duk_to_java_object %d   success %d", i, value == NULL);
	    	  (*env)->SetObjectArrayElement(env, args,  i, value);
					if(value != NULL){
		    	   (*env)->DeleteLocalRef(env, value);
					}
	 }
	 return args;
}



int duk_new_java_class(duk_context *ctx){
    int num = duk_get_top(ctx);
    DEBUG_LOG("ScriptEngine", "duk_new_java_class with args  num %d", num);
    duk_push_this(ctx);
		duk_get_prop_string(ctx, -1, JAVA_CLASS_NAME_KEY);
		const char*  className = duk_to_string(ctx, -1);
		JNIEnv*  env =  get_java_jni_env();
		jstring  fullClassName = (*env)->NewStringUTF(env, className);
		//FIXME  和invoke统一
    jobject  instance =  NULL;
    if(num == 0){
       	  instance = (*env)->CallStaticObjectMethod(env, java_api_class, java_new_instance_method, fullClassName, NULL);
    }else{
    	    jobjectArray args = duk_to_java_object_array(ctx, 0, num, env);
    	    instance  = (*env)->CallStaticObjectMethod(env, java_api_class, java_new_instance_method, fullClassName, args, NULL);
    	    (*env)->DeleteLocalRef(env, args);
    }
		if((*env)->ExceptionCheck(env)){
				jthrowable exp = ( *env)->ExceptionOccurred(env);
				if(exp != NULL){
									( *env)->ExceptionClear(env);
									jstring exceptionMessage = (*env)->CallStaticObjectMethod(env, java_api_class, java_exception_get_stack_trace_method, exp);
									jboolean isCopy = JNI_FALSE;
									const char* cstrMessage = (*env)->GetStringUTFChars(env, exceptionMessage, &isCopy);
									duk_push_error_object(ctx, DUK_ERR_EVAL_ERROR, "duk_new_java_class exception \n %s", cstrMessage);
									if(instance != NULL){
										 (*env)->DeleteLocalRef(env, instance);
									}
									(*env)->DeleteLocalRef(env, fullClassName);
									duk_throw(ctx);
									return 0;
				}
		}
		DEBUG_LOG("ScriptEngine", "duk_new_java_class CallStaticObjectMethod success %p", instance);
		if(instance != NULL){
	      duk_mark_jsobject_to_java_object(ctx, -2, env, instance);
		    (*env)->DeleteLocalRef(env, instance);
		}
    (*env)->DeleteLocalRef(env, fullClassName);
    duk_pop_2(ctx);
	  return 0;
}




void duk_android_fatal_function(duk_context *ctx, duk_errcode_t code, const char *msg){
	    LOGE("ScriptEngine","ScriptEngine javascript engine fatal error code %d  msg %s", code, msg);
      abort();
}

static duk_ret_t duk_android_log_print(duk_context *ctx) {
	int n = duk_get_top(ctx);
	switch (n) {
	case 1:
			LOGD("ScriptEngine", "%s", duk_safe_to_string(ctx, 0));
			break;
	case 2:
		    LOGD("ScriptEngine", "%s %s", duk_safe_to_string(ctx, 0),
				duk_safe_to_string(ctx, 1));
		    break;
	case 3:
		    LOGD("ScriptEngine", "%s %s %s", duk_safe_to_string(ctx, 0),
				duk_safe_to_string(ctx, 1), duk_safe_to_string(ctx, 2));
		    break;
	case 4: {
		    LOGD("ScriptEngine", "%s %s %s %s", duk_safe_to_string(ctx, 0),
				duk_safe_to_string(ctx, 1), duk_safe_to_string(ctx, 2),
				duk_safe_to_string(ctx, 3));
	     }
		   break;
	case 5: {
			    LOGD("ScriptEngine", "%s %s %s %s %s", duk_safe_to_string(ctx, 0),
					duk_safe_to_string(ctx, 1), duk_safe_to_string(ctx, 2),
					duk_safe_to_string(ctx, 3), duk_safe_to_string(ctx, 4));
		     }
		  	break;
	case 6: {
					LOGD("ScriptEngine", "%s %s %s %s %s %s", duk_safe_to_string(ctx, 0),
							duk_safe_to_string(ctx, 1), duk_safe_to_string(ctx, 2),
							duk_safe_to_string(ctx, 3), duk_safe_to_string(ctx, 4),
							duk_safe_to_string(ctx, 5));
				     }
					break;
	case 0: {
			   	LOGD("ScriptEngine", "\n");
			  }
				break;
	default: {
				LOGW("ScriptEngine", "too many args %d to print, javascript print only support less than 7 args\n",
						n);
		 	}
			break;
	}
	return 0;
}

JNIEXPORT jlong JNICALL Java_com_furture_react_DuktapeEngine_nativeInit(JNIEnv *env, jobject thisObject){
	env = get_java_jni_env();
	duk_context *ctx  = duk_create_heap(NULL, NULL, NULL, NULL, &duk_android_fatal_function);
	if(ctx){
		  duk_js_ref_setup(ctx);

		  duk_push_global_object(ctx);
		  duk_push_c_function(ctx, duk_android_log_print, DUK_VARARGS);
	    duk_put_prop_string(ctx, -2, "print");

	    duk_push_c_function(ctx, duk_import_java_class, DUK_VARARGS);
	    duk_put_prop_string(ctx, -2, "importClass");


    	 void* enginePtr = (*env)->NewGlobalRef(env, thisObject);
    	 duk_push_pointer(ctx, enginePtr);
    	 duk_put_prop_string(ctx, -2, JAVA_ENGINE_MARK);


	      duk_get_prototype(ctx, -1);
	      duk_push_c_function(ctx, invoke_java_method_call, DUK_VARARGS);
	      duk_put_prop_string(ctx, -2, JAVA_METHOD_MARK);

	      duk_push_c_function(ctx, duk_java_property_get, DUK_VARARGS);
	    	duk_put_prop_string(ctx, -2, JAVA_GET_MARK);


	    	duk_push_c_function(ctx, duk_java_property_set, DUK_VARARGS);
	    	duk_put_prop_string(ctx, -2, JAVA_SET_MARK);
		    duk_pop(ctx);

	    	duk_pop(ctx);
	    	DEBUG_LOG("ScriptEngine", "Native Init Success \n");
		return convert_to_ptr(ctx);
	}
	return -1;
}


JNIEXPORT void JNICALL Java_com_furture_react_DuktapeEngine_nativeRegister
  (JNIEnv *env, jobject thisObject, jlong ptr, jstring key, jobject value){
	env = get_java_jni_env();
	duk_context *ctx  = convert_to_context(ptr);
	jboolean iscopy = JNI_FALSE;
	const char* src =  ((*env)->GetStringUTFChars(env, key, &iscopy));
	duk_push_global_object(ctx);
	duk_push_java_object(ctx, env, value);
 	duk_put_prop_string(ctx, -2, src);
	duk_pop(ctx);
	(*env)->ReleaseStringUTFChars(env, key, src);
}


JNIEXPORT jobject JNICALL Java_com_furture_react_DuktapeEngine_nativeExeclute
  (JNIEnv *env, jobject thisObject, jlong  ptr, jstring script){
	env = get_java_jni_env();
	duk_context *ctx  = convert_to_context(ptr);
	jboolean iscopy = JNI_FALSE;
	const char* src = ((*env)->GetStringUTFChars(env, script, &iscopy));
	if(duk_peval_string(ctx, src) != DUK_EXEC_SUCCESS){
		    LOGE("ScriptEngine", "ScriptEngine eval_string error %s\n", duk_js_error_to_string(ctx, -1));
			  if(src != NULL){
				  	env = get_java_jni_env();
		 	     (*env)->ReleaseStringUTFChars(env, script, src);
		 	 }
	     return  NULL;
	}
	if(src != NULL){
	    (*env)->ReleaseStringUTFChars(env, script, src);
	}
	return duk_to_java_object(ctx, env, -1);
}

JNIEXPORT jobject JNICALL Java_com_furture_react_DuktapeEngine_nativeCallJs
  (JNIEnv *env, jobject thisObject, jlong ptr, jstring jsTarget, jstring jsMethod, jobjectArray args){
	  env = get_java_jni_env();
	  duk_context *ctx  = convert_to_context(ptr);
	  jboolean iscopy = JNI_FALSE;
    const char* targetName = ((*env)->GetStringUTFChars(env, jsTarget, &iscopy));
    DEBUG_LOG("ScriptEngine","Java_com_furture_react_DuktapeEngine_nativeCallJs call on %s", targetName);
	  duk_push_global_object(ctx);
    if(duk_get_prop_string(ctx, -1, targetName)){
			if(duk_is_function(ctx, -1)){
				jsize length = duk_push_java_object_array(ctx, env, args);
				DEBUG_LOG("ScriptEngine","Java_com_furture_react_DuktapeEngine_nativeCallJs call  Function %d", length);
				if(duk_pcall(ctx, length) != DUK_EXEC_SUCCESS){
					  LOGE("ScriptEngine","ScriptEngine CallJS function %s  %s", targetName, duk_js_error_to_string(ctx, -1));
						duk_pop(ctx);
						duk_push_null(ctx);
				}
				(*env)->ReleaseStringUTFChars(env, jsTarget, targetName);
				jobject  value =  duk_to_java_object(ctx, env, -1);
				duk_pop_2(ctx);
				return value;
			}else{
				const char* methodName = ((*env)->GetStringUTFChars(env, jsMethod, &iscopy));
				duk_push_string(ctx, methodName);
				jsize length = duk_push_java_object_array(ctx, env, args);
				DEBUG_LOG("ScriptEngine","Java_com_furture_react_DuktapeEngine_nativeCallJs call  Function %d", length);
				if(duk_pcall_prop(ctx, -2 - length, length) != DUK_EXEC_SUCCESS){
					  LOGE("ScriptEngine","ScriptEngine CallJS %s.%s() method %s", targetName, methodName, duk_js_error_to_string(ctx, -1));
						duk_pop(ctx);
						duk_push_null(ctx);
				}
				(*env)->ReleaseStringUTFChars(env, jsTarget, targetName);
				(*env)->ReleaseStringUTFChars(env, jsMethod, methodName);
				jobject  value =  duk_to_java_object(ctx, env, -1);
				duk_pop_2(ctx);
				return value;
			}
    }
    (*env)->ReleaseStringUTFChars(env, jsTarget, targetName);
    duk_pop_2(ctx);
    return NULL;

}

JNIEXPORT jobject JNICALL Java_com_furture_react_DuktapeEngine_nativeCallJSRef
  (JNIEnv *env, jobject thisObject, jlong ptr, jint jsRef, jstring proxyMethod, jobjectArray args){
	 env = get_java_jni_env();
   duk_context *ctx  = convert_to_context(ptr);
   duk_push_js_ref(ctx, jsRef);
   if(duk_is_function(ctx, -1)){
	    jsize length = duk_push_java_object_array(ctx, env, args);
	    if(duk_pcall(ctx, length) != DUK_EXEC_SUCCESS){
			  jboolean iscopy = JNI_FALSE;
			  const char* method = ((*env)->GetStringUTFChars(env, proxyMethod, &iscopy));
			  LOGE("ScriptEngine","ScriptEngine CallJSRef   %s method exception %s", method, duk_js_error_to_string(ctx, -1));
			  duk_pop(ctx);
			  duk_push_null(ctx);
			  if(method != NULL){
			     (*env)->ReleaseStringUTFChars(env, proxyMethod, method);
			  }
		  }
		  DEBUG_LOG("ScriptEngine","Java_com_furture_react_DuktapeEngine_nativeCallJSRef call  Function %d", length);
		  jobject  value =  duk_to_java_object(ctx, env, -1);
		  duk_pop(ctx);
		  return value;
   }
   jboolean iscopy = JNI_FALSE;
   const char* method = ((*env)->GetStringUTFChars(env, proxyMethod, &iscopy));
   duk_push_string(ctx, method);
   jsize length = duk_push_java_object_array(ctx, env, args);
   DEBUG_LOG("ScriptEngine","Java_com_furture_react_DuktapeEngine_nativeCallJSRef call Object  Function %d", length);
   if(duk_pcall_prop(ctx, -2 - length, length) != DUK_EXEC_SUCCESS){
	   LOGE("ScriptEngine","ScriptEngine CallJSRef  %d proxy method %s exception %s", jsRef, method, duk_js_error_to_string(ctx, -1));
	   duk_pop(ctx);
	   duk_push_null(ctx);
   }
   (*env)->ReleaseStringUTFChars(env, proxyMethod, method);
   DEBUG_LOG("ScriptEngine","Java_com_furture_react_DuktapeEngine_nativeCallJSRef call Object success %d", duk_get_type(ctx, -1));
   jobject  value =  duk_to_java_object(ctx, env, -1);
   duk_pop_2(ctx);
   return value;
}


JNIEXPORT void JNICALL Java_com_furture_react_DuktapeEngine_nativeFinalizeJSRef
  (JNIEnv *env, jobject thisObject, jlong ptr, jint ref){
	 env = get_java_jni_env();
	 duk_context *ctx  = convert_to_context(ptr);
	 if(ctx == NULL || ref == 0 || !ctx){
		 return;
	 }
	 duk_push_js_ref(ctx, ref);
	 if(duk_get_prop_string(ctx, -1, JS_REF_MARK)){
			 duk_del_prop_string(ctx, -2, JS_REF_MARK);
	         jweak  weakRef = duk_to_pointer(ctx, -1);
			 if(weakRef != NULL){
  	      (*env)->DeleteWeakGlobalRef(env, weakRef);
				  weakRef = NULL;
			 }
	 }
	 duk_js_unref(ctx, ref);
	 duk_pop_2(ctx);
}




JNIEXPORT void JNICALL Java_com_furture_react_DuktapeEngine_nativeDestory
  (JNIEnv * env, jobject thisObject, jlong ptr){
	env = get_java_jni_env();
	duk_context *ctx  = convert_to_context(ptr);
	if(ctx){
        jobject enginePtr  = get_engine_from_context(ctx);
         duk_push_js_refs(ctx);
         duk_enum(ctx, -1, 0);
         while (duk_next(ctx, -1 /*enum_index*/, 0 /*get_value*/)) {
            duk_dup(ctx, -1);
            duk_get_prop(ctx, -4);
            if(duk_is_object(ctx, -1)){
                if(duk_get_prop_string(ctx, -1, JS_REF_MARK)){
                      duk_del_prop_string(ctx, -2, JS_REF_MARK);
                      jweak  weakRef = duk_to_pointer(ctx, -1);
                      if(weakRef != NULL){
                          (*env)->DeleteWeakGlobalRef(env, weakRef);
                      }
                 }
                 duk_pop(ctx);
            }
            duk_pop(ctx);
            /* [ ... enum key ] */
            duk_del_prop(ctx, -3);
         }
         duk_pop_2(ctx);
         if(enginePtr != NULL){
            (*env)->DeleteGlobalRef(env, enginePtr);
         }
         duk_destroy_heap(ctx);
         ctx = NULL;
	}
}

static jclass findGlobalRefClass(JNIEnv* env, const char* className){
	 jclass cls = (*env)->FindClass(env, className);
	 jclass global_ref_class = (*env)->NewGlobalRef(env, cls);
	(*env)->DeleteLocalRef(env, cls);
	return global_ref_class;
}




JNIEnv*  get_java_jni_env()
{

        JNIEnv* env;
		 env = pthread_getspecific(jni_env_key);
		 if(env != NULL){
			  return env;
		 }
	    JavaVMAttachArgs args = {JNI_VERSION_1_6, 0, 0};
	    //pthread_t thread =  pthread_self();
	    //pthread_getname_np();
      (*jvm)->AttachCurrentThread(jvm, &env, &args);
		 if(env == NULL){
			   LOGE("ScriptEngine","Duktape Engine ScriptEngine AttachCurrentThread  error ");
				 return NULL;
		 }
		 pthread_setspecific(jni_env_key, env);
     return env;
}

void clean_jni_env(void* ptr){
	JNIEnv* env = (JNIEnv*)ptr;
	if(env && env != NULL){
		  DEBUG_LOG("ScriptEngine","ScriptEngine javascript DetachCurrentThread");
      (*jvm)->DetachCurrentThread(jvm);
	}
}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
	jvm  = vm;
	pthread_key_create (&jni_env_key, clean_jni_env);
	JNIEnv* 	env = NULL;
	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		env = get_java_jni_env(vm);
		if(env == NULL){
			return -1;
		}
	}
	if (java_api_class == NULL) {
		   java_api_class =  findGlobalRefClass(env, "com/furture/react/JavaUtils");
	}

	if (java_function_method == NULL) {
		java_function_method =
				(*env)->GetStaticMethodID(env, java_api_class, "invoke",
						"(Ljava/lang/Object;Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;");
	}

	if (java_field_get_method == NULL) {
		java_field_get_method = (*env)->GetStaticMethodID(env, java_api_class,
				"get", "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;");
	}

	if (java_import_class_method == NULL) {
		java_import_class_method = (*env)->GetStaticMethodID(env,
				java_api_class, "importClass", "(Ljava/lang/String;)Ljava/lang/Class;");
	}

	if (java_field_set_method == NULL) {
		java_field_set_method = (*env)->GetStaticMethodID(env, java_api_class,
				"set", "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/Object;)V");
	}

	if (java_new_instance_method == NULL) {
		java_new_instance_method = (*env)->GetStaticMethodID(env,
				java_api_class, "newInstance", "(Ljava/lang/String;[Ljava/lang/Object;)Ljava/lang/Object;");
	}

	if (java_exception_get_stack_trace_method == NULL) {
		java_exception_get_stack_trace_method = (*env)->GetStaticMethodID(env, java_api_class, "getStackTrace",
				"(Ljava/lang/Throwable;)Ljava/lang/String;");
	}

	if (js_ref_class == NULL) {
		   js_ref_class = findGlobalRefClass(env, "com/furture/react/JSRef");
	}

	if (js_ref_new_method == NULL) {
		 js_ref_new_method = (*env)->GetMethodID(env, js_ref_class, "<init>", "(Lcom/furture/react/DuktapeEngine;I)V");
	}

	if (js_ref_get_ref_method == NULL) {
		 js_ref_get_ref_method = (*env)->GetMethodID(env, js_ref_class, "getRef", "()I");
	}

	if(java_boolean_class == NULL){
		 java_boolean_class= findGlobalRefClass(env, "java/lang/Boolean");
	}

	if(java_integer_class  == NULL){
		java_integer_class = findGlobalRefClass(env, "java/lang/Integer");
	}
	if(java_long_class  == NULL){
		java_long_class = findGlobalRefClass(env, "java/lang/Long");
	}

	if(java_double_class  == NULL){
		java_double_class = findGlobalRefClass(env, "java/lang/Double");
	}

	if(java_float_class  == NULL){
		java_float_class = findGlobalRefClass(env, "java/lang/Float");
	}

	if(java_short_class  == NULL){
		java_short_class = findGlobalRefClass(env, "java/lang/Short");
	}

	if(java_byte_class == NULL){
		java_byte_class = findGlobalRefClass(env, "java/lang/Byte");
	}

	if(java_number_class == NULL){
		java_number_class = findGlobalRefClass(env, "java/lang/Number");
	}

	if(java_lang_object_class == NULL){
		 java_lang_object_class = findGlobalRefClass(env, "java/lang/Object");
	}

	if(java_number_get_double_value_method  == NULL){
		java_number_get_double_value_method = (*env)->GetMethodID(env, java_number_class, "doubleValue", "()D");
	}

	if(java_boolean_get_boolean_value_method == NULL){
		java_boolean_get_boolean_value_method = (*env)->GetMethodID(env, java_boolean_class, "booleanValue", "()Z");
	}

  if(java_boolean_constructor_method == NULL){
		 java_boolean_constructor_method = (*env)->GetMethodID(env, java_boolean_class, "<init>", "(Z)V");
	}

	if(java_double_constructor_method == NULL){
		  java_boolean_constructor_method = (*env)->GetMethodID(env, java_double_class, "<init>", "(D)V");
	}

  if(java_lang_to_string_method == NULL){
		  java_lang_to_string_method = (*env)->GetMethodID(env, java_lang_object_class, "toString", "()Ljava/lang/String;");
	}




	DEBUG_LOG("ScriptEngine","JNI_OnLoad Success");
	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved) {
	JNIEnv* env = NULL;
	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		env = get_java_jni_env(vm);
		if(env == NULL){
			return;
		}
	}
	if (java_api_class != NULL) {
		 (*env)->DeleteGlobalRef(env, java_api_class);
		 java_api_class = NULL;
	}

	if (js_ref_class != NULL) {
		  (*env)->DeleteGlobalRef(env, java_api_class);
			js_ref_class = NULL;
	}

	if (java_boolean_class != NULL) {
		 (*env)->DeleteGlobalRef(env, java_api_class);
		 java_boolean_class = NULL;
	}

	if (java_integer_class != NULL) {
		 (*env)->DeleteGlobalRef(env, java_integer_class);
		 java_integer_class = NULL;
	}
	if (java_long_class != NULL) {
		 (*env)->DeleteGlobalRef(env, java_long_class);
		 java_long_class = NULL;
	}

	if (java_double_class != NULL) {
		 (*env)->DeleteGlobalRef(env, java_double_class);
		 java_double_class = NULL;
	}

	if (java_float_class != NULL) {
		 (*env)->DeleteGlobalRef(env, java_float_class);
		 java_float_class = NULL;
	}

	if (java_short_class != NULL) {
		 (*env)->DeleteGlobalRef(env, java_short_class);
		 java_short_class = NULL;
	}

	if (java_byte_class != NULL) {
		(*env)->DeleteGlobalRef(env, java_byte_class);
		java_byte_class  = NULL;
	}

	if (java_number_class != NULL) {
		(*env)->DeleteGlobalRef(env, java_number_class);
		java_number_class = NULL;
	}

   if(java_lang_object_class != NULL){
		  (*env)->DeleteGlobalRef(env, java_lang_object_class);
 		  java_lang_object_class = NULL;
	 }

	 if(java_number_get_double_value_method != NULL){
		 java_number_get_double_value_method = NULL;
	 }

	 if(java_boolean_get_boolean_value_method != NULL){
 		 java_boolean_get_boolean_value_method =  NULL;
 	 }

	 if(java_boolean_constructor_method != NULL){
 		  java_boolean_constructor_method =  NULL;
 	 }

	 if(java_double_constructor_method != NULL){
 		  java_boolean_constructor_method = NULL;
 	 }

   if(java_lang_to_string_method != NULL){
 		  java_lang_to_string_method = NULL;
 	 }

   pthread_setspecific(jni_env_key, NULL);
	 jvm = NULL;
	 DEBUG_LOG("ScriptEngine","JNI_OnUnload Success");
}
