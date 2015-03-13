
function similarity = conceptor_similarity (a, b)
       
	[U_c S_c V_c] = svd(a);
	[U_p S_p V_p] = svd(b);
	% similarity with previous conceptor
	similarity = ...
	         norm((sqrt(S_c) * U_c' * ...
	         U_p * sqrt(S_p)),'fro')^2 / ...
	         (norm(a,'fro') * norm(b,'fro'));
end