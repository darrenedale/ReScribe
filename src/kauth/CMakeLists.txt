add_executable(rescribe_imagewriter_helper imagewriterhelper.cpp)
target_compile_features(rescribe_imagewriter_helper PRIVATE cxx_std_17)
target_link_libraries(rescribe_imagewriter_helper
        KF5::AuthCore
        ${LibArchive_LIBRARIES}
        )

# the helper is going into /usr/local/lib/kauth/ but (on Manjaro) it should be /usr/lib/kauth/
install(TARGETS rescribe_imagewriter_helper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
kauth_install_helper_files(rescribe_imagewriter_helper net.equituk.rescribe.imagewriter root)
kauth_install_actions(net.equituk.rescribe.imagewriter imagewriter.actions)
