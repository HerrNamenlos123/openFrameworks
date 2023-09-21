define_addon(ofxOpenCv)

find_package(OpenCV REQUIRED)
target_link_libraries(ofxOpenCv of::opencv)

if (WIN32)      # We only need ippicv on Windows
    target_link_libraries(ofxOpenCv of::ippicv)
endif()