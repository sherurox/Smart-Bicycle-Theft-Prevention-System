function decodeUplink(input) {
    var bytes = input.bytes;

    // Decode state (1 byte)
    var state = (bytes[0] === 1) ? "LOCKED" : "UNLOCKED";

    // Decode latitude (next 4 bytes)
    var latitude = ((bytes[1] << 24) | (bytes[2] << 16) | (bytes[3] << 8) | bytes[4]) / 1000000;

    // Decode longitude (next 4 bytes)
    var longitude = ((bytes[5] << 24) | (bytes[6] << 16) | (bytes[7] << 8) | bytes[8]) / 1000000;

    return {
        data: {
            state: state,
            location: {
                latitude: latitude,
                longitude: longitude
            }
        },
        warnings: [],
