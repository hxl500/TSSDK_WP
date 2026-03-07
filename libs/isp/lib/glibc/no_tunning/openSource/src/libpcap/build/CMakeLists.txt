################################################ libpcap  ###################################################
if (${ARMCB_CROSS_BUILD} EQUAL 1)

	if (ARMCB_AARCH64_BUILD EQUAL 1)
		if (BUILD_OPTION_UCLIBC EQUAL 0)
			set (LIBPCAP_PATH ${ARMCB_ISP_EXTERNAL}/libpcap/libpcap-1.9.1/lib64/arm-linux/glibc)
		else ()
			MESSAGE( STATUS "uclibc=${BUILD_OPTION_UCLIBC}")
			set (LIBPCAP_PATH ${ARMCB_ISP_EXTERNAL}/libpcap/libpcap-1.9.1/lib64/arm-linux/uclibc)
		endif()
	else ()
		if (BUILD_OPTION_UCLIBC EQUAL 0)
			set (LIBPCAP_PATH ${ARMCB_ISP_EXTERNAL}/libpcap/libpcap-1.9.1/lib/arm-linux/glibc)
		else ()
			MESSAGE( STATUS "uclibc=${BUILD_OPTION_UCLIBC}")
			set (LIBPCAP_PATH ${ARMCB_ISP_EXTERNAL}/libpcap/libpcap-1.9.1/lib/arm-linux/uclibc)
		endif()
	endif()

	if (ARMCB_STATIC_COMPILE EQUAL 0)
		set (LIB_PCAP
			${LIBPCAP_PATH}/libpcap.so.1.9.1
			${LIBPCAP_PATH}/libpcap.so.1
			${LIBPCAP_PATH}/libpcap.so
			)
	else ()
		set (LIB_PCAP
			${LIBPCAP_PATH}/libpcap.a)
	endif ()

	file(COPY ${LIB_PCAP} DESTINATION ${ARMCB_EXTERNAL_LIB})

elseif (${ARMCB_CROSS_BUILD} EQUAL 0)

    set (LIB_PCAP
        ${ARMCB_ISP_EXTERNAL}/libpcap/libpcap-1.9.1/lib64/linux/libpcap.so.1.9.1
        ${ARMCB_ISP_EXTERNAL}/libpcap/libpcap-1.9.1/lib64/linux/libpcap.so.1
        ${ARMCB_ISP_EXTERNAL}/libpcap/libpcap-1.9.1/lib64/linux/libpcap.so
    )
    file(COPY ${LIB_PCAP} DESTINATION ${ARMCB_EXTERNAL_LIB})

endif()

