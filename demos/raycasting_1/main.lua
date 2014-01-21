
print("Loading main.lua script.")

function mainloop_on_start()
    --print("mainloop_on_start()")
    mainloop.set_renderer("SwRenderer")
    mainloop.set_world("")
    
    world.set_level_size(12, 12)
    world.set_level_layout({
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 3, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    })
    
    player.set_position(5.5, 5.5)
end

function mainloop_on_quit()
    print("mainloop_on_quit()")
end

function mainloop_on_input()
    --print("mainloop_on_input()")
end

function mainloop_on_update()
    --print("mainloop_on_update()")
end

function mainloop_on_render()
    --print("mainloop_on_render()")
end
