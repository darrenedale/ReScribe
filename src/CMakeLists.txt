add_executable(rescribe src/main.cpp src/imagechooser.cpp src/imagechooser.h src/mainwindow.cpp src/mainwindow.h src/devicechooser.cpp src/devicechooser.h src/imagewriter.cpp src/imagewriter.h)
target_compile_features(rescribe PRIVATE cxx_std_17)
target_link_libraries(rescribe
        Qt5::Core
        Qt5::Widgets
        KF5::WidgetsAddons
        KF5::KIOCore
        KF5::KIOFileWidgets
        KF5::KIOWidgets
        KF5::KIONTLM
        KF5::Solid
        KF5::AuthCore
        KF5::Auth
        )

set_target_properties(rescribe PROPERTIES
        PROJECT_LABEL ReScribe
        AUTOUIC_SEARCH_PATHS ui
        )

include_directories(kauth)
