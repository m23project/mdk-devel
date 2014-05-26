#!/usr/bin/php
<?php





/**
**name isUpper($char)
**description Checks, if a character is upper case
**parameter char: Character to check.
**returns true, when upper case otherwise false
**/
function isUpper($char)
{
	$ord = ord($char);
	return(($ord > 64) && ($ord < 91));
}





/**
**name keyAndRelease($keyCode, $pressShift)
**description Generates (Shift press,) key, key release (and Shift release) codes.
**parameter keyCode: Key (scan) code.
**parameter pressShift: true, when Shift should be pressed.
**returns (Shift press,) key, key release (and Shift release)
**/
function keyAndRelease($keyCode, $pressShift)
{
	// Add Shift press and release codes?
	if ($pressShift)
	{
		$shiftBegin = '2a ';
		$shiftEnd = ' aa';
	}
	else
		$shiftEnd = $shiftBegin = '';


	// Calculate the release code
	$rel = dechex(hexdec($keyCode) + 128);

	// Return (Shift press,) key, key release (and Shift release)
	return("$shiftBegin$keyCode $rel$shiftEnd");
}





/**
**name getKey($charIn)
**description Generates the needed scan codes to produce a given character.
**parameter charIn: Input character.
**returns Needed scan codes to produce a given character.
**/
function getKey($charIn)
{
	$pressShift = isUpper($charIn);
	$charIn = strtolower($charIn);

	// Table with characters that need Shift to be pressed
	$withShift['!'] = '02';
	$withShift['@'] = '03';
	$withShift['#'] = '04';
	$withShift['$'] = '05';
	$withShift['%'] = '06';
	$withShift['^'] = '07';
	$withShift['&'] = '08';
	$withShift['*'] = '09';
	$withShift['['] = '0a';
	$withShift[')'] = '0b';
	$withShift['-'] = '0c';
	$withShift['+'] = '0d';
	$withShift['{'] = '1a';
	$withShift['}'] = '1b';
	$withShift[':'] = '27';
	$withShift['"'] = '28';
	$withShift['~'] = '29';
	$withShift['|'] = '2b';
	$withShift['<'] = '33';
	$withShift['>'] = '34';
	$withShift['?'] = '35';

	$keyCode['esc'] = '01';
	$keyCode['1'] = '02';
	$keyCode['2'] = '03';
	$keyCode['3'] = '04';
	$keyCode['4'] = '05';
	$keyCode['5'] = '06';
	$keyCode['6'] = '07';
	$keyCode['7'] = '08';
	$keyCode['8'] = '09';
	$keyCode['9'] = '0a';
	$keyCode['0'] = '0b';
	$keyCode['-'] = '0c';
	$keyCode['='] = '0d';
	$keyCode['backspace'] = '0e';
	$keyCode['tab'] = '0f';
	$keyCode['q'] = '10';
	$keyCode['w'] = '11';
	$keyCode['e'] = '12';
	$keyCode['r'] = '13';
	$keyCode['t'] = '14';
	$keyCode['y'] = '15';
	$keyCode['u'] = '16';
	$keyCode['i'] = '17';
	$keyCode['o'] = '18';
	$keyCode['p'] = '19';
	$keyCode['{'] = '1a';
	$keyCode['}'] = '1b';
	$keyCode['enter'] = '1c';
	$keyCode['lctrl'] = '1d';
	$keyCode['a'] = '1e';
	$keyCode['s'] = '1f';
	$keyCode['d'] = '20';
	$keyCode['f'] = '21';
	$keyCode['g'] = '22';
	$keyCode['h'] = '23';
	$keyCode['j'] = '24';
	$keyCode['k'] = '25';
	$keyCode['l'] = '26';
	$keyCode[':'] = '27';
	$keyCode['"'] = '28';
	$keyCode['~'] = '29';
	$keyCode['lshift'] = '2a';
	$keyCode['|'] = '2b';
	$keyCode['z'] = '2c';
	$keyCode['x'] = '2d';
	$keyCode['c'] = '2e';
	$keyCode['v'] = '2f';
	$keyCode['b'] = '30';
	$keyCode['n'] = '31';
	$keyCode['m'] = '32';
	$keyCode['<'] = '33';
	$keyCode['>'] = '34';
	$keyCode['?'] = '35';
	$keyCode['rshift'] = '36';
	$keyCode['keypad-*'] = '27';
	$keyCode['lalt'] = '38';
	$keyCode['space bar'] = '39';
	$keyCode['capslock'] = '3a';
	$keyCode['f1'] = '3b';
	$keyCode['f2'] = '3c';
	$keyCode['f3'] = '3d';
	$keyCode['f4'] = '3e';
	$keyCode['f5'] = '3f';
	$keyCode['f6'] = '40';
	$keyCode['f7'] = '41';
	$keyCode['f8'] = '42';
	$keyCode['f9'] = '43';
	$keyCode['f10'] = '44';
	$keyCode['numlock'] = '45';
	$keyCode['scrolllock'] = '46';
	$keyCode['keypad-7/home'] = '47';
	$keyCode['keypad-8/up'] = '48';
	$keyCode['keypad-9/pgup'] = '49';
	$keyCode['keypad--'] = '4a';
	$keyCode['keypad-4/left'] = '4b';
	$keyCode['keypad-5'] = '4c';
	$keyCode['keypad-6/right'] = '4d';
	$keyCode['keypad-+'] = '4e';
	$keyCode['keypad-1/end'] = '4f';
	$keyCode['keypad-2/down'] = '50';
	$keyCode['keypad-3/pgdn'] = '51';
	$keyCode['keypad-0/ins'] = '52';
	$keyCode['keypad-./del'] = '53';
	$keyCode['alt-sysrq'] = '54';

	// Check, if the character is reachable without Shift
	$key = @$keyCode[$charIn];

	if ($key !== NULL)
		return(keyAndRelease($key, $pressShift));
	else
	{
		// Search in the array with Shift presses
		$key = @$withShift[$charIn];
		if ($key !== NULL)
			return(keyAndRelease($key, true));
		else
			die("Unbekannt: $charIn");
	}
}

	$in = $argv[1];

	// Run thru the characters if the input string
	for ($i = 0; $i < strlen($in); $i++)
	{
		$char = $in[$i];
		echo(getKey($char).' ');
	}

	// Press/release Enter
	echo("1c 9c\n");
?>