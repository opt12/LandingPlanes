import { propOr, identity } from 'ramda';

// see http://randycoulman.com/blog/2016/02/16/using-ramda-with-redux/ for explanation of createReducer
const createReducer = (initialState, handlers) =>
  (state = initialState, action) =>
    propOr(identity, action.type, handlers)(state, action);

export default createReducer;
