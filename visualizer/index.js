import { html, render } from 'https://unpkg.com/lit-html@0.7.1/lib/lit-extended.js'
import Store from './store.js'
import Header from './header.js'
import reducer from './reducers.js'
import { changeName, set_ast } from './actions.js'
import { ScopeViewer } from './ScopeViewer.js'

export const store = new Store(reducer, {
  name: '',
  ast: {
    name: 'unit',
    children: []
  }
})

fetch('./tree.json')
.then(function(response) {
  return response.json();
})
.then(function(json) {
  set_ast(json)
}).catch(error => {
  console.log(error)
});


const App = state => {
  return html`
    ${ScopeViewer(state.ast)}
  `;
};

export function force_update() {
  render(App(store.state), document.querySelector('body'))
}

store.suscribe(state => force_update());
