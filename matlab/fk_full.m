 

function ret = fk_full(q)
	ret = zeros(2,3);
	ret(1,3) = cos(q(1)) + cos(q(1) + q(2)) + cos(q(1) + q(2) + q(3));
	ret(2,3) = sin(q(1)) + sin(q(1) + q(2)) + sin(q(1) + q(2) + q(3));
	ret(1,2) = cos(q(1)) + cos(q(1) + q(2));
	ret(2,2) = sin(q(1)) + sin(q(1) + q(2));
	ret(1,1) = cos(q(1));
	ret(2,1) = sin(q(1));
end
