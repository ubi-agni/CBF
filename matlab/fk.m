
function ret = fk(q)
	ret = zeros(2,1);
	ret(1) = cos(q(1)) + cos(q(1) + q(2)) + cos(q(1) + q(2) + q(3));
	ret(2) = sin(q(1)) + sin(q(1) + q(2)) + sin(q(1) + q(2) + q(3));
end
