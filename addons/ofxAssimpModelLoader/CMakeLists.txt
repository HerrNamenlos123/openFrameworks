define_addon(ofxAssimpModelLoader)

find_package(assimp REQUIRED)
target_link_libraries(ofxAssimpModelLoader assimp::assimp)