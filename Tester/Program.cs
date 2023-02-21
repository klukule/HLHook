using HogwartsLegacy;

Hook hook = new Hook();
if (!hook.Initialize())
{
    while (true)
    {
        //hook.GetPlayerPositionAndRotation(out var position, out var rotation);
        //Console.WriteLine($"X={position.X}, Y={position.Y}, Z={position.Z}; Yaw={rotation.Yaw}, Pitch={rotation.Pitch}, Roll={rotation.Roll}");
        hook.GetPlayerPositionAndRotationOverwolf((pos, rot) =>
        {
            var p = (MFVector)pos;
            var r = (MFRotator)rot;
            Console.WriteLine($"X={p.X}, Y={p.Y}, Z={p.Z}; Yaw={r.Yaw}, Pitch={r.Pitch}, Roll={r.Roll}");
        });
        Thread.Sleep(100);
    }
}
