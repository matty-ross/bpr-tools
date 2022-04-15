import struct


def Ignore(data: bytes) -> None:
    pass


def ChangeVehicleColor(data: bytes) -> None:
    palette_index = struct.unpack("<L", data[0x0:0x4])[0]
    color_index = struct.unpack("<L", data[0x4:0x8])[0]
    print(f"change color  ->  {palette_index = } | {color_index = }")


# ================================


HANDLERS = {
    5: ChangeVehicleColor,
    53: Ignore,
    70: Ignore,
    72: Ignore,
    75: Ignore,
    76: Ignore,
    77: Ignore,
    78: Ignore,
    79: Ignore,
    82: Ignore,
    84: Ignore,
    86: Ignore,
    87: Ignore,
    88: Ignore,
    90: Ignore,
    98: Ignore,
    117: Ignore,
    131: Ignore,
    135: Ignore,
    137: Ignore,
    138: Ignore
}


def HandleGameEvent(event_type: int, event_size: int, event_data: bytes) -> None:
    assert len(event_data) == event_size
    
    handler = HANDLERS.get(event_type)
    if handler:
        handler(event_data)
        return

    # unknown event
    print(f"{event_type :3} [{event_size :4X}]  {event_data.hex(' ').upper()}")