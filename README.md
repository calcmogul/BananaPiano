# Banana Piano

This project is a piano that can use any objects as its keys. This was made for the 2015 Project Design Lab class at Allan Hancock College and is inspired by the Makey Makey.

* [Arduino](Arduino) contains the driver code for an Arudino's analog input pins.
* [BeagleBone](BeagleBone) contains the driver code for a BeagleBone Black's analog input pins.
* [3DCapacitorBananaPianoJournal.pdf](3DCapacitorBananaPianoJournal.pdf) has a more detailed project description, pictures, and electrical schematics.

## Desktop application

### Build

```bash
cmake -B build
cmake --build build
cmake --install build --prefix app
```

### Usage

The desktop application opens a small window with a circle indicating the serial port connection status.

| Color  | Status                              |
|--------|-------------------------------------|
| Red    | Disconnected                        |
| Yellow | Connected but received invalid data |
| Green  | Connected and received valid data   |

The desktop app will automatically connect to an Arduino or BeagleBone that's plugged in.
