package("eigen")

    set_kind("library", {headeronly = true})
    set_homepage("https://eigen.tuxfamily.org/")
    set_description("C++ template library for linear algebra")
    set_license("MPL-2.0")

    add_urls("https://gitlab.com/libeigen/eigen/-/archive/$(version)/eigen-$(version).tar.bz2",
             "https://gitlab.com/libeigen/eigen.git")
    add_versions("3.4.0", "b4c198460eba6f28d34894e3a5710998818515104d6e74e5cc331ce31e46e626")

--     if is_plat("mingw") and is_subhost("msys") then
--         add_extsources("pacman::eigen3")
--     elseif is_plat("linux") then
--         add_extsources("pacman::eigen", "apt::libeigen3-dev")
--     elseif is_plat("macosx") then
--         add_extsources("brew::eigen")
--     end

    add_deps("cmake")
    add_includedirs("include")
    add_includedirs("include/eigen3")

    on_install(function (package)
        io.replace("CMakeLists.txt", "add_subdirectory", "#", {plain = true})
        import("package.tools.cmake").install(package, {"-DBUILD_TESTING=OFF"})
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <iostream>
            #include <Eigen/Dense>
            using Eigen::MatrixXd;
            void test()
            {
                MatrixXd m(2,2);
                m(0,0) = 3;
                m(1,0) = 2.5;
                m(0,1) = -1;
                m(1,1) = m(1,0) + m(0,1);
                std::cout << m << std::endl;
            }
        ]]}, {configs = {languages = "c++11"}}))
    end)