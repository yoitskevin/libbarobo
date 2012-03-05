//
//  iMobotController_MacAppDelegate.m
//  iMobotController_Mac
//
//  Created by dko on 2/28/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "iMobotController_MacAppDelegate.h"

@implementation iMobotController_MacAppDelegate

@synthesize window;
@synthesize configWindow;
@synthesize addressesTableView;
@synthesize addAddressTextField;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application 
	configFile = [[ConfigFile alloc] init];
	[configFile initWithFilename:@"/Users/dko/.Barobo.config"];
	[addressesTableView setDataSource:configFile];
}

/* Menu Handlers */
- (IBAction) onMenuConfigureConfigure:(id)sender {
	/* Open the configure dialog window */
	[addressesTableView reloadData];
	//[configWindow display];
	[configWindow orderFront:self];
}

/* Config Dialog Button Handlers */
- (IBAction) onButtonConfigOK:(id)sender {
	[configFile writeFile];
	[configWindow orderOut:self];
}

- (IBAction) onButtonConfigAdd:(id)sender {
	/* Get the text entry */
	NSString *textEntry = [addAddressTextField stringValue];
	[configFile addAddress:textEntry];
	[addressesTableView reloadData];
}

- (IBAction) onButtonConfigRemove:(id)sender {
	NSInteger rowIndex;
	rowIndex = [addressesTableView selectedRow];
	if(rowIndex < 0) {
		return;
	}
	[configFile removeAtIndex:rowIndex];
	[addressesTableView reloadData];
}

- (IBAction) onButtonMoveDown:(id)sender {
	NSInteger rowIndex;
	rowIndex = [addressesTableView selectedRow];
	if(rowIndex < 0) return;
	[configFile moveAddressDown:rowIndex];
	[addressesTableView reloadData];
}

- (IBAction) onButtonMoveUp:(id)sender {
	NSInteger rowIndex;
	rowIndex = [addressesTableView selectedRow];
	if(rowIndex < 0) return;
	[configFile moveAddressUp:rowIndex];
	[addressesTableView reloadData];
}

@end