find_package(LibArchive REQUIRED COMPONENTS LibArchive)

add_executable(rescribe main.cpp imagechooser.cpp imagechooser.h mainwindow.cpp mainwindow.h devicechooser.cpp devicechooser.h progresswidget.cpp progresswidget.h application.cpp application.h sizetextgenerator.cpp diskimageinfofactory.cpp diskimageinfofactory.h diskimageinfo.h isodiskimageinfo.cpp isodiskimageinfo.h writeconfirmationwidget.cpp writeconfirmationwidget.h writeconfirmationdialogue.cpp writeconfirmationdialogue.h)

target_compile_features(rescribe PRIVATE cxx_std_17)

target_link_libraries(rescribe
        Qt5::Core
        Qt5::Widgets
        Qt5::DBus
        KF5::WidgetsAddons
        KF5::KIOCore
        KF5::KIOFileWidgets
        KF5::KIOWidgets
        KF5::KIONTLM
        KF5::Solid
        KF5::AuthCore
        KF5::Auth
        ${LibArchive_LIBRARIES}
        )

set_target_properties(rescribe PROPERTIES
        PROJECT_LABEL ReScribe
        AUTOUIC_SEARCH_PATHS ui
        )

add_subdirectory(kauth)
