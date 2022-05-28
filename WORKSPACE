load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest.git",
    commit = "703bd9caab50b139428cea1aaff9974ebee5742e",
    shallow_since = "1570114335 -0400",
)

git_repository(
    name = "com_github_nelhage_rules_boost",
    commit = "9f9fb8b2f0213989247c9d5c0e814a8451d18d7f",
    remote = "https://github.com/nelhage/rules_boost",
    shallow_since = "1570056263 -0700",
)
load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
boost_deps()

http_archive(
    name = "json",
    build_file_content = """
cc_library(
        name = "json",
        hdrs = ["single_include/nlohmann/json.hpp"],
        visibility = ["//visibility:public"],
        strip_include_prefix = "single_include/"
)
""",
    urls = ["https://github.com/nlohmann/json/releases/download/v3.7.3/include.zip"],
    sha256 = "87b5884741427220d3a33df1363ae0e8b898099fbc59f1c451113f6732891014",
)

git_repository(
    name = "optimizationtools",
    remote = "https://github.com/fontanf/optimizationtools.git",
    commit = "592ba85b762fce7333542335a4fcad0e88582492",
    shallow_since = "1650692671 +0200",
)

local_repository(
    name = "optimizationtools_",
    path = "../optimizationtools/",
)

git_repository(
    name = "localsearchsolver",
    remote = "https://github.com/fontanf/localsearchsolver.git",
    commit = "479547553b666084ac981fe82478e752826714b7",
    shallow_since = "1648967658 +0200",
)

local_repository(
    name = "localsearchsolver_",
    path = "../localsearchsolver/",
)

git_repository(
    name = "columngenerationsolver_",
    remote = "https://github.com/fontanf/columngenerationsolver.git",
    commit = "e1f129fb7f912996d98bcefe1ba42d84ad5a62c9",
    shallow_since = "1649493214 +0200",
)

local_repository(
    name = "columngenerationsolver",
    path = "../columngenerationsolver/",
)

git_repository(
    name = "knapsacksolver",
    remote = "https://github.com/fontanf/knapsacksolver.git",
    commit = "619f8a65a9c23eaab9825dcce589408945e7ef65",
    shallow_since = "1649492589 +0200",
)

http_archive(
    name = "dlib",
    build_file_content = """
cc_library(
        name = "dlib",
        hdrs = [
                "dlib-19.19/dlib/all/source.cpp",
        ] + glob(["**/*.h"], exclude_directories = 0),
        visibility = ["//visibility:public"],
        strip_include_prefix = "dlib-19.19/",
)
""",
    urls = ["http://dlib.net/files/dlib-19.19.tar.bz2"],
    sha256 = "1decfe883635ce51acd72869cebe870ab9b85eb094d417adc8f48aa7b8c60cd7",
)

new_local_repository(
    name = "coinor",
    path = "/home/florian/Programmes/coinbrew/",
    build_file_content = """
cc_library(
    name = "coinor",
    hdrs = glob(["dist/include/**/*.h*"], exclude_directories = 0),
    strip_include_prefix = "dist/include/",
    srcs = glob(["dist/lib/**/*.so"], exclude_directories = 0),
    visibility = ["//visibility:public"],
)
""",
)

new_local_repository(
    name = "cplex",
    path = "/opt/ibm/ILOG/CPLEX_Studio129/",
    build_file_content = """
cc_library(
    name = "concert",
    hdrs = glob(["concert/include/ilconcert/**/*.h"], exclude_directories = 0),
    strip_include_prefix = "concert/include/",
    srcs = ["concert/lib/x86-64_linux/static_pic/libconcert.a"],
    linkopts = [
            "-lm",
            "-lpthread",
            "-ldl",
    ],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "cplex",
    hdrs = glob(["cplex/include/ilcplex/*.h"]),
    strip_include_prefix = "cplex/include/",
    srcs = [
            "cplex/lib/x86-64_linux/static_pic/libilocplex.a",
            "cplex/lib/x86-64_linux/static_pic/libcplex.a",
    ],
    deps = [":concert"],
    visibility = ["//visibility:public"],
)
cc_library(
    name = "cpoptimizer",
    hdrs = glob(["cpoptimizer/include/ilcp/*.h"]),
    strip_include_prefix = "cpoptimizer/include/",
    srcs = ["cpoptimizer/lib/x86-64_linux/static_pic/libcp.a"],
    deps = [":cplex"],
    visibility = ["//visibility:public"],
)
""",
)

new_local_repository(
    name = "gurobi",
    path = "/home/florian/Programmes/gurobi811/linux64/",
    build_file_content = """
cc_library(
    name = "gurobi",
    hdrs = [
            "include/gurobi_c.h",
            "include/gurobi_c++.h",
    ],
    strip_include_prefix = "include/",
    srcs = [
            "lib/libgurobi_c++.a",
            "lib/libgurobi81.so",
    ],
    visibility = ["//visibility:public"],
)
""",
)

new_local_repository(
    name = "gecode",
    path = "/home/florian/Programmes/gecode-release-6.2.0/",
    build_file_content = """
cc_library(
    name = "gecode",
    hdrs = glob(["gecode/**/*.h*"], exclude_directories = 0),
    srcs = [
            "libgecodedriver.so",
            "libgecodeflatzinc.so",
            "libgecodefloat.so",
            "libgecodegist.so",
            "libgecodeint.so",
            "libgecodekernel.so",
            "libgecodeminimodel.so",
            "libgecodesearch.so",
            "libgecodeset.so",
            "libgecodesupport.so",
    ],
    visibility = ["//visibility:public"],
)
""",
)

new_local_repository(
    name = "localsolver",
    path = "/opt/localsolver_8_5",
    build_file_content = """
cc_library(
    name = "localsolver",
    hdrs = glob(["include/**/*.h"], exclude_directories = 0),
    strip_include_prefix = "include/",
    srcs = glob(["lib/**/*.a", exclude_directories = 0]),
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
)
""",
)

new_local_repository(
    name = "knitro",
    path = "/home/florian/Programmes/knitro-13.1.0-z-Linux-Intel17-64/",
    build_file_content = """
cc_library(
    name = "knitro",
    hdrs = [
            "include/knitro.h",
    ],
    strip_include_prefix = "include/",
    srcs = [
            "lib/libknitro.so",
            "lib/libiomp5.so",
    ],
    copts = [
            "-fopenmp",
    ],
    linkopts = [
            "-fopenmp",
            "-ldl",
            "-liomp5",
    ],
    visibility = ["//visibility:public"],
)
""",
)
