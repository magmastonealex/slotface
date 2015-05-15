import math

for t in range(12):
	xflip=1
	yflip=1
	if t > 3 and t =< 6:
		yflip=-1
		t=t-3

	if xflip==1 and yflip==1:
		t=3-t
		