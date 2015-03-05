function [out] = ridgeregression(input, target, alpha)
   [size_rows, size_cols] = size(input);
   
   % do it step by step
   corr = input * input'
   alph_i = alpha * eye(size_rows)
   inner_add = corr + alph_i
   inv_done = inv(inner_add)
   input_target = input*target'
   outer_prod = inv_done*input_target
   final = outer_prod'
   
   out = (inv(input * input' + ...
    alpha * eye(size_rows)) ...
    * input * target')';
    
    