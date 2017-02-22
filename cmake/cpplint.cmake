cmake_minimum_required(VERSION 2.8)

add_custom_target(cpplint
    COMMAND cpplint --recursive --filter=-legal/copyright,-build/namespaces --linelength=120 ${PROJECT_SOURCE_DIR}/{src,include,example}/ )

