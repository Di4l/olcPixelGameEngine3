# Extensions
In PGE3, an extension allows the easy addition of automated functionality or wrapping of 3rd party libraries. Extensions **ADD** features by hooking into the main system loop and being called by PGE3 at various points.

Extensions live in the `olc::ext` namespace.

For example, the SplashScreen extension simply requires the presence of an `olc::ext::SplashScreen` object, and PGE3 will take care of the sequencing and rendering, removing that burden from the user. More complicated extensions such as audio may comprise several files or indeed require the installation of additional libraries.

Extension code should exist in a self-contained and self-named folder, along with a `README.md` detailing their use and installation requirements. No 3rd party code should be stored within the folder, and we expect the README to sufficiently inform the user how to acquire, configure and use any 3rd part libraries.

Support for extensions is not guaranteed, as extension maintainers come and go, 3rd party libraries change over time and the requirements for the extension may disappear.

If you have an extension idea or contribution, then please reach out via our Discord server.

`TODO: Basic Extension Writing Guide`