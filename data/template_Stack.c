bool Stack_init(Stack* stack){
	stack->count = 0;
	stack->size = 1;
	stack->v = malloc(sizeof(*stack->v) * stack->size);
	return (stack->v != NULL);
}


bool Stack_delete(Stack* stack){
	stack->count = 0;
	stack->size = 0;
	free(stack->v);
}


bool Stack_rezerve(Stack* stack, int newSize){
	if (newSize > stack->size){
		
		const size_t size = sizeof(void*) * newSize;
		void* v = realloc(stack->v, size);
		
		if (v == NULL){
			return false;
		}
		
		stack->v = v;
		stack->size = newSize;
		return true;
	}
	return true;
}


bool Stack_push(Stack* stack, void* value){
	const int i = stack->count;
	
	if (i >= stack->size){
		if (!Stack_rezerve(stack, STACK_RESIZERULE(stack->size)))
			return false;
	}
	
	stack->v[i] = value;
	stack->count++;
	
	return true;
}


void* Stack_pop(Stack* stack){
	const int i = --stack->count;
	return stack->v[i];
}