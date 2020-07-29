a = array.new(10)
print(a)
print('size :',a:size())
for i=1,10 do
	a:set(i,i/10)	
end

print(a:__tostring())
print(a:get(1))
