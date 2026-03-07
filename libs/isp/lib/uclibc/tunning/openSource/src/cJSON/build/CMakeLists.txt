################################################libcjson ###########################################################
set(LIB_CJSON cjson)

add_library ( ${LIB_CJSON} ${ARMCB_LIB_FLAG}
    ${ARMCB_ISP_EXTERNAL}/cJSON/cJSON.c
)

################################################# Install src ########################################################
#set (CJSON_SRC
#    ${ARMCB_ISP_EXTERNAL}/cJSON/cJSON.c
#)
#
#install(FILES ${CJSON_SRC} DESTINATION ${ARMCB_GENERATED_OUT_JSON_SRC})
################################################# Install header ######################################################
#set (CJSON_INC
#    ${ARMCB_ISP_EXTERNAL}/cJSON/cJSON.h
#)
#
#install(FILES ${CJSON_INC} DESTINATION ${ARMCB_GENERATED_OUT_JSON_INC})

############################################### Install target ###############################################
if ( (ARMCB_PKG_EXTERNAL EQUAL 1) OR (ARMCB_CUST_BUILD EQUAL 1) )
    install (TARGETS ${LIB_CJSON}
             DESTINATION ${ARMCB_EXTERNAL_LIB})
endif ()

set (ENV{ARMCB_ALL_LIBS} "$ENV{ARMCB_ALL_LIBS} -l${LIB_CJSON}")
