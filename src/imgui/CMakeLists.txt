
set(IMGUI_SOURCES
    "imgui.cpp"
    "imgui_draw.cpp"
    "imgui_tables.cpp"
    "imgui_widgets.cpp"
    "imgui-SFML.cpp"
    "imgui_demo.cpp"
)

target_sources(neogroubiks
    PUBLIC ${IMGUI_SOURCES}
)