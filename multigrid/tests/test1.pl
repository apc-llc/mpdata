#!/usr/bin/perl -w

my($nx_min) = 1; my($nx_max) = 10;
my($ny_min) = 1; my($ny_max) = 10;
my($ns_min) = 1; my($ns_max) = 10;

my($sx_min) = 1; my($sx_max) = 10;
my($sy_min) = 1; my($sy_max) = 10;
my($ss_min) = 1; my($ss_max) = 10;

for ($inx = $nx_min; $inx < $nx_max; $inx++)
{
	for ($iny = $ny_min; $iny < $ny_max; $iny++)
	{
		for ($ins = $ns_min; $ins < $ns_max; $ins++)
		{
			for ($isx = $sx_min; $isx < $sx_max; $isx++)
			{
				for ($isy = $sy_min; $isy < $sy_max; $isy++)
				{
					for ($iss = $ss_min; $iss < $ss_max; $iss++)
					{
						system("./test1 $inx $iny $ins $isx $isy $iss >> test1.out");
					}
				}
			}		
		}
	}
}
