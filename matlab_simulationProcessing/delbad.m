function posdata=delbad(posdata,x,y)

delrows=[]

j=1

for i=2:size(posdata,1)

	if  ( sqrt((posdata(i,x)-posdata(j,x)).^2+(posdata(i,y)-posdata(j,y)).^2) < 100)
		j=i;
	else
		
		delrows=[delrows;i];
	endif
	
endfor

posdata(delrows,:)=[];