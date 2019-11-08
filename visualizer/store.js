export default class Store {
    constructor(reducer, initial) {
      this.state = initial || {}
      this.reducer = reducer
      this.listeners = []
    }

    dispatch(action) {
      const old_state = this.state;
      this.state = this.reducer(this.state, action);
      if (old_state !== this.state)
        for (let listener of this.listeners)
          listener(this.state);
    }

    suscribe(listener) {
      this.listeners.push(listener);
      listener(this.state);
    }
}
