define_addon(ofxSvg)

find_package(libxml2 REQUIRED)
target_link_libraries(ofxSvg of::svgtiny libxml2::libxml2)