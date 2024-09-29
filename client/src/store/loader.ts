import { create } from "zustand";

type LoaderState = {
  isLoading: boolean;
  setIsLoading: (loading: boolean) => void;
};

export const useLoaderStore = create<LoaderState>((set) => ({
  isLoading: false,
  setIsLoading: (loading) => set({ isLoading: loading }),
}));
