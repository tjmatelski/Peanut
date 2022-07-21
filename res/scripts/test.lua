function OnUpdate (timestep)
    if IsKeyPressed(KeyCode.W) then
        transform.position.y = transform.position.y + 0.01
    end
    if IsKeyPressed(KeyCode.A) then
        transform.position.x = transform.position.x - 0.01
    end
    if IsKeyPressed(KeyCode.S) then
        transform.position.y = transform.position.y - 0.01
    end
    if IsKeyPressed(KeyCode.D) then
        transform.position.x = transform.position.x + 0.01
    end
end