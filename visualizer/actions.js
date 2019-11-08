import { store } from './index.js'

export const CHANGE_NAME = 0;
export const TOGGLE_CHILDREN = 1;
export const SET_AST = 2;

export function changeName(name) {
  store.dispatch({
    type: CHANGE_NAME,
    name: name
  })
}

export function set_ast(json) {
  store.dispatch({
    type: SET_AST,
    ast: json
  })
}


export function toggle_children(scope_data) {
  store.dispatch({
    type: TOGGLE_CHILDREN,
    scope_data: scope_data
  })
}
