# Device tree overlays

## Uses of overlays (a patch which overlays the main dtb)
- To support and managed hardware configuration (properties, nodes, pin configurations) of various capes of the board
- To alter the properties of already existing device nodes of the main dtb
- Overlays approach maintains modularity and makes capes management easier

## Overlay DTS Format
````
The DTS of an overlay should have the following format:

{
	/* ignored properties by the overlay */

	fragment@0 {	/* first child node */

		target=<phandle>;	/* phandle target of the overlay */
	or
		target-path="/path";	/* target path of the overlay */

		__overlay__ {
			property-a;	/* add property-a to the target */
			node-a {	/* add to an existing, or create a node-a */
				...
			};
		};
	}
	fragment@1 {	/* second child node */
		...
	};
	/* more fragments follow */
}
````