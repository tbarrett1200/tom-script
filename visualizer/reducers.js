import { CHANGE_NAME, TOGGLE_CHILDREN, SET_AST } from './actions.js'

export default function reducer(state, action) {
  switch(action.type) {
    case CHANGE_NAME:
      return Object.assign({}, state, {
        name: action.name
      })
    case TOGGLE_CHILDREN:
      action.scope_data.hide_children = !action.scope_data.hide_children
      return Object.assign({}, state);
    case SET_AST:
      return Object.assign({}, state, {
        ast: action.ast
      });
    default:
      return state;
  }
}
