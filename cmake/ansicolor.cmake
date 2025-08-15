message(STATUS "载入颜色控制")

string(ASCII 27 Esc)
set(Reset "${Esc}[m")
set(ColourBold "${Esc}[1m")
set(Red "${Esc}[31m")
set(Green "${Esc}[32m")
set(Yellow "${Esc}[33m")
set(Blue "${Esc}[34m")
set(Magenta "${Esc}[35m")
set(Cyan "${Esc}[36m")
set(White "${Esc}[37m")
set(BoldRed "${Esc}[1;31m")
set(BoldGreen "${Esc}[1;32m")
set(BoldYellow "${Esc}[1;33m")
set(BoldBlue "${Esc}[1;34m")
set(BoldMagenta "${Esc}[1;35m")
set(BoldCyan "${Esc}[1;36m")
set(BoldWhite "${Esc}[1;37m")

message(STATUS "${Red}颜${Green}色${Blue}测${BoldWhite}试${Reset}")

function(info TEXT)
    if(WIN32 AND NOT CYGWIN)
        # Windows 平台（非 Cygwin）通常不支持 ANSI 颜色
        message(STATUS "${TEXT}")
    else()
        # 使用 ANSI 转义码显示红色文本
        message(STATUS "${Red}${TEXT}${Reset}")
    endif()
endfunction()


info("this is a test cmake funciton output")