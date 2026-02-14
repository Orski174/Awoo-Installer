# Awoo Installer
A No-Bullshit NSP, NSZ, XCI, and XCZ Installer for Nintendo Switch

![Awoo Installer Main Menu](https://i.imgur.com/q5Qff0R.jpg)

## Features
- Installs NSP/NSZ/XCI/XCZ files and split NSP/XCI files from your SD card
- Installs NSP/NSZ/XCI/XCZ files over LAN or USB from tools such as [NS-USBloader](https://github.com/developersu/ns-usbloader)
- Installs NSP/NSZ/XCI/XCZ files over the internet by URL or Google Drive
- Crawls and browses HTTP/HTTPS URLs to find and install files from web directories
- Verifies NCAs by header signature before they're installed
- Installs and manages the latest signature patches quickly and easily
- Based on [Adubbz Tinfoil](https://github.com/Adubbz/Tinfoil)
- Uses [XorTroll's Plutonium](https://github.com/XorTroll/Plutonium) for a pretty graphical interface
- Just werks

## Why?
Because Goldleaf tends to not "Just werk" when installing NSP files. I wanted a *free software* solution that installs, looks pretty, and doesn't make me rip my hair out whenever I want to put software on my Nintendo Switch. Awoo Installer does exactly that. It installs software. That's about it!

If you want to do other things like manage installed tickets, titles, and user accounts, check out [Goldleaf](https://github.com/XorTroll/Goldleaf)!

## Installation from HTTP/HTTPS URLs

Awoo Installer supports three methods for installing over the internet:

1. **Direct URL**: Enter a direct link to an NSP/NSZ/XCI/XCZ file
2. **Google Drive**: Enter a Google Drive file ID for public files
3. **Browse URL** (NEW): Enter a directory URL and Awoo will automatically crawl and find all installable files

### Using the Browse URL Feature

The Browse URL feature makes it easy to install games from web servers with directory listings:

1. Select "Install over LAN or internet" from the main menu
2. Press **Y** to install over the internet
3. Select **"Browse URL"** from the options
4. Enter a URL to a directory (e.g., `http://example.com/games/`)
5. Awoo Installer will automatically:
   - Crawl the URL and subdirectories (up to 2 levels deep)
   - Find all NSP, NSZ, XCI, and XCZ files
   - Display them in a selection menu
6. Select the files you want to install and press **+**

This feature works with any web server that provides HTML directory listings, making it much easier to install multiple games without having to manually enter each URL.

## Thanks to
- HookedBehemoth for A LOT of contributions
- Adubbz and other contributors for [Tinfoil](https://github.com/Adubbz/Tinfoil)
- XorTroll for [Plutonium](https://github.com/XorTroll/Plutonium) and [Goldleaf](https://github.com/XorTroll/Goldleaf)
- blawar (wife strangulator) and nicoboss for [NSZ](https://github.com/nicoboss/nsz) support
- The kind folks at the AtlasNX Discuck (or at least some of them)
- The also kind folks at the RetroNX Discuck (of no direct involvement)
- [namako8982](https://www.pixiv.net/member.php?id=14235616) for the Momiji art
- TheXzoron for being a baka
