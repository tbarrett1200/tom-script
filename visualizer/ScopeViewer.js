import { svg } from 'https://unpkg.com/lit-html@0.7.1/lit-html.js'
import { html } from 'https://unpkg.com/lit-html@0.7.1/lib/lit-extended.js'
import { repeat } from 'https://unpkg.com/lit-html@0.7.1/lib/repeat.js';
import { toggle_children } from './actions.js';

const triangle_right = svg`
  <svg width="8" height="8">
  <polygon points="1,0,7,4,1,8"></polygon>
  </svg>
`
const triangle_down = svg`
  <svg width="8" height="8">
  <polygon points="0,1,8,1,4,7"></polygon>
  </svg>
`
const triangle_none = svg`
  <svg width="8" height="8"></svg>
`
export function ScopeViewer(unit_scope) {
  return html`
    <div class='scope-viewer'>
    <h1>Scope</h1>
    ${Scope(unit_scope)}
    </div>
  `
}

export function Scope(scope_data) {
  return html`
    <div class='scope'>
      <div class='scope-header' on-click=${() => toggle_children(scope_data)}>
        ${ scope_data.children && scope_data.children.length > 0
          ? (scope_data.hide_children ? triangle_right: triangle_down)
          : triangle_none
         }
        <span class="scope-kind"> ${scope_data.kind}</span>
        <span class="scope-name">${scope_data.name}</span>
        <span class="scope-type">${scope_data.type}</span>
      </div>
      <div class$="${scope_data.hide_children? "hidden scope-list": "scope-list"}">
      ${repeat(
        scope_data.children,
        scope => scope.id,
        scope => Scope(scope)
      )}
      </div>
    </div>
  `
}
