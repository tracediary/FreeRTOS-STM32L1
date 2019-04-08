编译产品可执行文件步骤：
	
	1 配置产品硬件板子：
		./user/Version_Contrl/Hardware_Version/Hardware_Version.h
		#define HARDWARE_VERSION 		CSF_JG_V1_2/CSF_NSYT_V1_0/ ... 
		
	1 配置产品类型：
		./usr/sys.h 
		#define PRODUCT_CONTRL  		SMART_DRAINAGE/SMART_COVER/ ...

	2 配置产品功能：
		./usr/sys.h 
		#define 	BT_FUNCTION_CONFIG	NO/YES
		#define 	NETACTION_CONFIG	NO/YES
	