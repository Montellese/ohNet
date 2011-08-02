#include <jni.h>
#include <malloc.h>
#include <stdlib.h>
#include "CpProxy.h"
#include "JniCallbackList.h"
#include "OpenHome/Net/C/CpProxy.h"

#ifdef __cplusplus
extern "C" {
#endif

static JniCallbackList *iList = NULL;

static void STDCALL CallbackChange(void* aPtr)
{
	JniObjRef* ref = (JniObjRef*) aPtr;
	JNIEnv *env;
	jclass cls;
	jmethodID mid;
	jint ret;
	
	ret = (*(ref->vm))->AttachCurrentThread(ref->vm, (void **) &env, NULL);
	if (ret < 0)
		printf("Unable to attach thread to JVM.\n");
	
	cls = (*env)->GetObjectClass(env, ref->callbackObj);
	mid = (*env)->GetMethodID(env, cls, "notifyChange", "()V");
	if (mid == 0) {
		printf("Method ID notifyChange() not found.\n");
		return;
	}
	(*env)->CallVoidMethod(env, ref->callbackObj, mid);
	
	(*(ref->vm))->DetachCurrentThread(ref->vm);
}

static void STDCALL CallbackInitial(void* aPtr)
{
	JniObjRef* ref = (JniObjRef*) aPtr;
	JNIEnv *env;
	jclass cls;
	jmethodID mid;
	jint ret;

	ret = (*(ref)->vm)->AttachCurrentThread(ref->vm, (void **) &env, NULL);
	if (ret < 0)
		printf("Unable to attach thread to JVM.\n");
	
	cls = (*env)->GetObjectClass(env, ref->callbackObj);
	mid = (*env)->GetMethodID(env, cls, "notifyChange", "()V");
	if (mid == 0) {
		printf("Method ID notifyChange() not found.\n");
		return;
	}
	(*env)->CallVoidMethod(env, ref->callbackObj, mid);
	
	(*(ref->vm))->DetachCurrentThread(ref->vm);
}

static void STDCALL InitialiseReferences(JNIEnv *aEnv, jobject aObject, JniObjRef **aRef)
{
	jint ret;
	*aRef = (JniObjRef *)malloc(sizeof(JniObjRef));
	if (iList == NULL)
	{
		iList = JniCallbackListCreate();
	}

	ret = (*aEnv)->GetJavaVM(aEnv, &(*aRef)->vm);
	if (ret < 0) {
		printf("CpProxyJNI: Unable to get reference to the current Java VM.\n");
		fflush(stdout);
	}
	(*aRef)->callbackObj = (*aEnv)->NewWeakGlobalRef(aEnv, aObject);
	if ((*aRef)->callbackObj == NULL) {
		printf("CpProxyJNI: Callback object not stored.\n");
		fflush(stdout);
	}
	JniCallbackListAddElement(&iList, *aRef);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxyCreate
 * Signature: (Ljava/lang/String;Ljava/lang/String;IJ)J
 */
JNIEXPORT jlong JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxyCreate
  (JNIEnv *aEnv, jclass aClass, jstring aDomain, jstring aName, jint aVersion, jlong aDevice)
{
	CpDeviceC device = (CpDeviceC) (size_t)aDevice;
	const char* domain = (*aEnv)->GetStringUTFChars(aEnv, aDomain, NULL);
	const char* name = (*aEnv)->GetStringUTFChars(aEnv, aName, NULL);
	THandle proxy = CpProxyCreate(domain, name, aVersion, device);
	aClass = aClass;
	
	(*aEnv)->ReleaseStringUTFChars(aEnv, aDomain, domain);
	(*aEnv)->ReleaseStringUTFChars(aEnv, aName, name);
	
	return (jlong) proxy;
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxyDestroy
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxyDestroy
  (JNIEnv *aEnv, jclass aClass, jlong aProxy)
{
	THandle proxy = (THandle) (size_t)aProxy;
	aClass = aClass;
	
	JniCallbackListDestroy(aEnv, &iList);
	iList = NULL;
	CpProxyDestroy(proxy);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxyService
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxyService
  (JNIEnv *aEnv, jclass aClass, jlong aProxy)
{
	THandle proxy = (THandle) (size_t)aProxy;
	aEnv = aEnv;
	aClass = aClass;
	
	return (jlong) CpProxyService(proxy);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxySubscribe
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxySubscribe
  (JNIEnv *aEnv, jclass aClass, jlong aProxy)
{
	THandle proxy = (THandle) (size_t)aProxy;
	aEnv = aEnv;
	aClass = aClass;

	CpProxySubscribe(proxy);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxyUnsubscribe
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxyUnsubscribe
  (JNIEnv *aEnv, jclass aClass, jlong aProxy)
{
	THandle proxy = (THandle) (size_t)aProxy;
	aEnv = aEnv;
	aClass = aClass;
	
	CpProxyUnsubscribe(proxy);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxySetPropertyChanged
 * Signature: (JLorg/openhome/net/controlpoint/IPropertyChangeListener;)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxySetPropertyChanged
  (JNIEnv *aEnv, jclass aClass, jlong aProxy, jobject aCallback)
{
	THandle proxy = (THandle) (size_t)aProxy;
	OhNetCallback callback = &CallbackChange;
	JniObjRef *ref;
	aClass = aClass;
	
	InitialiseReferences(aEnv, aCallback, &ref);
	
	CpProxySetPropertyChanged(proxy, callback, ref);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxySetPropertyInitialEvent
 * Signature: (JLorg/openhome/net/controlpoint/IPropertyChangeListener;)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxySetPropertyInitialEvent
  (JNIEnv *aEnv, jclass aClass, jlong aProxy, jobject aCallback)
{
	THandle proxy = (THandle) (size_t)aProxy;
	OhNetCallback callback = (OhNetCallback) &CallbackInitial;
	JniObjRef *ref;
	aClass = aClass;
	
	InitialiseReferences(aEnv, aCallback, &ref);
	
	CpProxySetPropertyChanged(proxy, callback, NULL);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxyPropertyReadLock
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxyPropertyReadLock
  (JNIEnv *aEnv, jclass aClass, jlong aProxy)
{
	THandle proxy = (THandle) (size_t)aProxy;
	aEnv = aEnv;
	aClass = aClass;
	
	CpProxyPropertyReadLock(proxy);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxyPropertyReadUnlock
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxyPropertyReadUnlock
  (JNIEnv *aEnv, jclass aClass, jlong aProxy)
{
	THandle proxy = (THandle) (size_t)aProxy;
	aEnv = aEnv;
	aClass = aClass;
	
	CpProxyPropertyReadUnlock(proxy);
}

/*
 * Class:     org_openhome_net_controlpoint_CpProxy
 * Method:    CpProxyAddProperty
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_org_openhome_net_controlpoint_CpProxy_CpProxyAddProperty
  (JNIEnv *aEnv, jclass aClass, jlong aProxy, jlong aProperty)
{
	THandle proxy = (THandle) (size_t)aProxy;
	ServiceProperty property = (ServiceProperty) (size_t)aProperty;
	aEnv = aEnv;
	aClass = aClass;
	
	CpProxyAddProperty(proxy, property);
}

#ifdef __cplusplus
}
#endif