import struct


def Ignore(data: bytes) -> None:
    pass


def ChangeVehicleColor(data: bytes) -> None:
    palette_index = struct.unpack("<L", data[0x0:0x4])[0]
    color_index = struct.unpack("<L", data[0x4:0x8])[0]
    print(f"change vehicle color  ->  {palette_index = } | {color_index = }")


def SelectVehicle(data: bytes) -> None:
    vehicle_id = struct.unpack("<Q", data[0x0:0x8])[0]
    wheel_id = struct.unpack("<Q", data[0x8:0x10])[0]
    print(f"select vehicle  ->  {vehicle_id = :016X} | {wheel_id = :016X}")


def PowerParkResult(data: bytes) -> None:
    OUTCOMES = (
        "to be determined",
        "success",
        "failure"
    )
    outcome = struct.unpack("<l", data[0x0:0x4])[0]
    outcome = OUTCOMES[outcome]
    rating = struct.unpack("<l", data[0x4:0x8])[0]
    other_players_involved = struct.unpack("<l", data[0x8:0xC])[0]
    print(f"power park result  ->  {outcome = } | {rating = } | {other_players_involved = }")


def BoostTimeComplete(data: bytes) -> None:
    time = struct.unpack("<f", data[0x0:0x4])[0]
    print(f"boost time complete  ->  {time = :.2f}")


def Drifting(data: bytes) -> None:
    return
    distance = struct.unpack("<f", data[0x0:0x4])[0]
    print(f"drifting  ->  {distance = :.2f}")


# ================================


HANDLERS = {
    4: SelectVehicle,
    5: ChangeVehicleColor,
    53: Ignore,
    55: PowerParkResult,
    56: BoostTimeComplete,
    70: Ignore,
    72: Ignore,
    75: Drifting,
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
    
    # get a handler for the specific event
    handler = HANDLERS.get(event_type)
    if handler:
        handler(event_data)
        return

    # unknown event
    print(f"{event_type :3} [{event_size :4X}]  {event_data.hex(' ').upper()}")