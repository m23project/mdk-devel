<?php


$variables['AT_detectManager'] = 2;
$variables['AT_deleteClient'] = 1;



function evaluateRunIf($answer)
{
	global $variables;

	if (isset($answer['runIf']))
	{
		// Split the value of a runIf into variable name, operator and compare value
		preg_match_all('/([^<=> ]*)([<=> ]*)(.*)/', $answer['runIf'], $variablesA);
		$var	= $variablesA[1][0];
		$op		= $variablesA[2][0];
		$val2	= $variablesA[3][0];

		// Check, if a value is stored in the runtime variable array and get its value, if possible
		if (!isset($variables[$var]))
			return(false);
		$val1 = $variables[$var];

		switch($op)
		{
			case '>':
				return($val1 > $val2);
			case '==':
				return($val1 == $val2);
			case '<':
				return($val1 < $val2);
			case '<=':
				return($val1 <= $val2);
			case '>=':
				return($val1 >= $val2);
			default:
				die("Unknown operator \"$op\"!");
		}
	}
		return(true);
}



foreach (array('AT_detectManager>0', 'AT_deleteClient==1') as $runIf)
{
	print("RI: $runIf => ".serialize(evaluateRunIf(array('runIf' => $runIf)))."\n");
}


?>