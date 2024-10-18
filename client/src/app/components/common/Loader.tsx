"use client";

import { useLoaderStore } from "@/store/loader";

export const Loader = () => {
  const { isLoading } = useLoaderStore();

  return (
    <div
      hidden={!isLoading}
      className="absolute top-0 left-0 w-screen h-screen flex justify-center bg-black/10"
    >
      <span className="loading loading-spinner text-success md:w-40 h-auto" />
    </div>
  );
};
