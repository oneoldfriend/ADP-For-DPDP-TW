cmake_minimum_required (VERSION 2.8)

project (adp)

aux_source_directory(. DIR_SRCS)

add_executable(adp ${DIR_SRCS})